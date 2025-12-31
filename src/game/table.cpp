#include "table.h"
#include "player.h"

#include <algorithm>
#include <cassert>
#include <chrono>

Table::Table()
    : current_player(0), dealer(0), round_wind(Wind::East),
      wall_pointer(0), dead_wall_start(122), kan_count(0),
      honba(0), riichi_sticks(0), is_started(false), is_finished(false) {
    players.fill(nullptr);
    // 初始化随机数生成器
    auto seed = std::chrono::steady_clock::now().time_since_epoch().count();
    rng.seed(static_cast<unsigned>(seed));
}

Table::~Table() {
    // 玩家由外部管理，这里不删除
}

void Table::setPlayer(int seat, Player* player) {
    assert(seat >= 0 && seat < 4);
    players[seat] = player;
    if (player) {
        player->setTable(this, seat);
    }
}

Wind Table::getSeatWind(int seat) const {
    // 座位风向: 庄家为东，逆时针分配
    int wind_offset = (seat - dealer + 4) % 4;
    return static_cast<Wind>(wind_offset);
}

void Table::shuffleWall() {
    wall.clear();
    wall.reserve(136);
    for (int i = 0; i < 136; ++i) {
        wall.push_back(i);
    }
    std::shuffle(wall.begin(), wall.end(), rng);
}

void Table::dealTiles() {
    // 每人发13张牌
    for (int i = 0; i < 4; ++i) {
        if (players[i]) {
            TileIndexList hand_tiles;
            int start = i * 13;
            for (int j = 0; j < 13; ++j) {
                hand_tiles.push_back(wall[start + j]);
            }
            players[i]->initHand(hand_tiles, round_wind, getSeatWind(i));
        }
    }
    wall_pointer = 52;  // 52张已发出
    dead_wall_start = 122;  // 最后14张是王牌
}

void Table::initRound() {
    shuffleWall();
    dealTiles();
    current_player = dealer;
    kan_count = 0;
    is_started = true;
    is_finished = false;
}

void Table::nextPlayer() {
    current_player = (current_player + 1) % 4;
}

bool Table::isWallEmpty() const {
    return wall_pointer >= dead_wall_start;
}

TileIndex Table::drawTile() {
    if (isWallEmpty()) {
        return invalid_tile_index;
    }
    TileIndex tile = wall[wall_pointer++];
    if (callbacks.onDraw) {
        callbacks.onDraw(current_player, tile);
    }
    return tile;
}

TileIndex Table::drawFromDeadWall() {
    // 岭上摸牌 (杠后摸牌)
    if (kan_count >= 4) {
        return invalid_tile_index;
    }
    // 从王牌区摸牌
    TileIndex tile = wall[dead_wall_start + kan_count];
    kan_count++;
    return tile;
}

GameResult Table::playRound() {
    initRound();

    GameResult result;
    result.winner = -1;
    result.is_tsumo = false;
    result.from_player = -1;
    result.han = 0;
    result.fu = 0;
    result.score = 0;

    while (!isWallEmpty()) {
        Player* player = players[current_player];
        if (!player) {
            nextPlayer();
            continue;
        }

        if (callbacks.onTurnStart) {
            callbacks.onTurnStart(current_player);
        }

        // 摸牌
        TileIndex drawn = drawTile();
        if (drawn == invalid_tile_index) {
            break;  // 牌山空了
        }

        // 检查是否能自摸
        bool can_tsumo = player->canWin(drawn);
        bool can_ankan = player->canAnkan();
        bool can_riichi = false;  // TODO: 实现立直判定

        // 玩家决策
        int action = player->decideAction(drawn, can_tsumo, can_ankan, can_riichi);

        // 处理自摸
        if (action == static_cast<int>(Action::Win)) {
            result.winner = current_player;
            result.is_tsumo = true;
            result.from_player = -1;
            result.yaku = player->getHand()->calcYaku(drawn, true);
            // TODO: 计算番数和点数
            is_finished = true;
            if (callbacks.onGameEnd) {
                callbacks.onGameEnd(result);
            }
            return result;
        }

        // 处理暗杠
        if (action == static_cast<int>(Action::Ankan)) {
            // TODO: 处理暗杠
            TileIndex rinshan = drawFromDeadWall();
            // 继续当前玩家回合
            continue;
        }

        // 处理弃牌
        TileIndex discard_tile;
        if (action >= 0 && action < 136) {
            discard_tile = action;
        } else {
            // 默认打出摸到的牌
            discard_tile = drawn;
        }

        // 执行弃牌
        player->discard(discard_tile);
        if (callbacks.onDiscard) {
            callbacks.onDiscard(current_player, discard_tile);
        }

        // 检查其他玩家的响应 (荣和/碰/杠/吃)
        int response_result = checkResponses(discard_tile, current_player);

        if (response_result == static_cast<int>(Action::Win)) {
            // 有人荣和
            // result 在 checkResponses 中设置
            is_finished = true;
            return result;
        }

        if (response_result == static_cast<int>(Action::Pass)) {
            // 没人响应，下一个玩家
            nextPlayer();
        }
        // 如果有人吃/碰/杠，current_player 已在 checkResponses 中更新
    }

    // 流局
    result.winner = -1;
    is_finished = true;
    if (callbacks.onGameEnd) {
        callbacks.onGameEnd(result);
    }
    return result;
}

int Table::checkResponses(TileIndex discard, int from_seat) {
    // 响应优先级: 荣和 > 碰/杠 > 吃
    // 吃只能是下家

    std::array<int, 4> responses;
    responses.fill(static_cast<int>(Action::Pass));

    // 收集所有玩家的响应
    for (int i = 1; i <= 3; ++i) {
        int seat = (from_seat + i) % 4;
        Player* player = players[seat];
        if (!player) continue;

        bool can_chi = (i == 1) && player->canChi(discard);  // 只有下家能吃
        bool can_pon = player->canPon(discard);
        bool can_kan = player->canKan(discard);
        bool can_ron = player->canWin(discard);

        if (can_chi || can_pon || can_kan || can_ron) {
            responses[seat] = player->decideResponse(discard, from_seat, can_chi, can_pon, can_kan, can_ron);
        }
    }

    // 处理荣和 (最高优先级)
    for (int i = 1; i <= 3; ++i) {
        int seat = (from_seat + i) % 4;
        if (responses[seat] == static_cast<int>(Action::Win)) {
            // TODO: 处理荣和，设置 GameResult
            return static_cast<int>(Action::Win);
        }
    }

    // 处理碰/杠
    for (int i = 1; i <= 3; ++i) {
        int seat = (from_seat + i) % 4;
        if (responses[seat] == static_cast<int>(Action::Pon)) {
            // 执行碰
            if (callbacks.onMeld) {
                callbacks.onMeld(seat, static_cast<int>(Action::Pon), discard);
            }
            current_player = seat;
            return static_cast<int>(Action::Pon);
        }
        if (responses[seat] == static_cast<int>(Action::Kan)) {
            // 执行大明杠
            if (callbacks.onMeld) {
                callbacks.onMeld(seat, static_cast<int>(Action::Kan), discard);
            }
            current_player = seat;
            return static_cast<int>(Action::Kan);
        }
    }

    // 处理吃 (只有下家)
    int next_seat = (from_seat + 1) % 4;
    if (responses[next_seat] == static_cast<int>(Action::Chi)) {
        if (callbacks.onMeld) {
            callbacks.onMeld(next_seat, static_cast<int>(Action::Chi), discard);
        }
        current_player = next_seat;
        return static_cast<int>(Action::Chi);
    }

    return static_cast<int>(Action::Pass);
}

void Table::processDiscard(TileIndex tile) {
    Player* player = players[current_player];
    if (player) {
        player->discard(tile);
    }
    if (callbacks.onDiscard) {
        callbacks.onDiscard(current_player, tile);
    }
}
