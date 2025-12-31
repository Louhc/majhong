#include "room.h"
#include <algorithm>
#include <random>
#include <chrono>

// HumanPlayer 实现
HumanPlayer::HumanPlayer(Session* s, const std::string& name)
    : Player(name), session(s), pending_action(-1), action_ready(false) {
}

void HumanPlayer::setAction(int action) {
    pending_action = action;
    action_ready = true;
}

int HumanPlayer::decideAction(TileIndex drawn_tile, bool can_tsumo, bool can_ankan, bool can_riichi) {
    // 等待网络输入 (在实际实现中，这应该是异步的)
    // 这里简单返回打出摸到的牌
    if (action_ready) {
        action_ready = false;
        return pending_action;
    }
    return drawn_tile;  // 默认摸什么打什么
}

int HumanPlayer::decideResponse(TileIndex discard, int from_seat, bool can_chi, bool can_pon, bool can_kan, bool can_ron) {
    if (action_ready) {
        action_ready = false;
        return pending_action;
    }
    return static_cast<int>(Action::Pass);  // 默认过
}

// Room 实现
Room::Room(const std::string& id)
    : room_id(id), game_table(nullptr), state(RoomState::Waiting), player_count(0) {
    sessions.fill(nullptr);
    players.fill(nullptr);
}

Room::~Room() {
    for (int i = 0; i < 4; ++i) {
        delete players[i];
    }
    delete game_table;
}

int Room::findEmptySeat() const {
    for (int i = 0; i < 4; ++i) {
        if (sessions[i] == nullptr) {
            return i;
        }
    }
    return -1;
}

bool Room::addPlayer(Session* session) {
    if (state != RoomState::Waiting) {
        return false;
    }

    int seat = findEmptySeat();
    if (seat < 0) {
        return false;
    }

    sessions[seat] = session;
    session->setRoom(this, seat);
    player_count++;

    // 创建人类玩家对象
    players[seat] = new HumanPlayer(session, session->getName());

    return true;
}

void Room::removePlayer(Session* session) {
    for (int i = 0; i < 4; ++i) {
        if (sessions[i] == session) {
            sessions[i] = nullptr;
            delete players[i];
            players[i] = nullptr;
            player_count--;
            session->setRoom(nullptr, -1);
            break;
        }
    }
}

void Room::fillWithAI() {
    for (int i = 0; i < 4; ++i) {
        if (players[i] == nullptr) {
            std::string ai_name = "AI-" + std::to_string(i + 1);
            players[i] = new SimpleAI(ai_name);
        }
    }
}

void Room::setupCallbacks() {
    if (!game_table) return;

    GameCallbacks callbacks;

    callbacks.onDraw = [this](int seat, TileIndex tile) {
        // 通知对应玩家摸牌
        if (sessions[seat]) {
            GameMessage msg;
            msg.type = MessageType::YourTurn;
            msg.seat = seat;
            msg.tile = tile;
            sessions[seat]->send(msg.toJSON());
        }
    };

    callbacks.onDiscard = [this](int seat, TileIndex tile) {
        // 广播弃牌
        GameMessage msg;
        msg.type = MessageType::PlayerAction;
        msg.seat = seat;
        msg.action = tile;  // 弃牌动作
        msg.tile = tile;
        broadcast(msg.toJSON());
    };

    callbacks.onMeld = [this](int seat, int action, TileIndex tile) {
        // 广播副露
        GameMessage msg;
        msg.type = MessageType::PlayerAction;
        msg.seat = seat;
        msg.action = action;
        msg.tile = tile;
        broadcast(msg.toJSON());
    };

    callbacks.onGameEnd = [this](const GameResult& result) {
        state = RoomState::Finished;
        GameMessage msg;
        msg.type = MessageType::GameEnd;
        msg.seat = result.winner;
        // TODO: 添加更多结果信息
        broadcast(msg.toJSON());
    };

    game_table->setCallbacks(callbacks);
}

void Room::startGame() {
    if (state != RoomState::Waiting) {
        return;
    }

    // 用 AI 填充空位
    fillWithAI();

    // 创建牌桌
    game_table = new Table();
    for (int i = 0; i < 4; ++i) {
        game_table->setPlayer(i, players[i]);
    }

    setupCallbacks();

    state = RoomState::Playing;

    // 广播游戏开始
    GameMessage msg;
    msg.type = MessageType::GameStart;
    broadcast(msg.toJSON());

    // TODO: 在异步环境中运行游戏
    // 这里暂时同步运行
    // game_table->playRound();
}

void Room::handleAction(Session* session, int action, int tile) {
    if (state != RoomState::Playing) {
        return;
    }

    // 找到玩家座位
    int seat = -1;
    for (int i = 0; i < 4; ++i) {
        if (sessions[i] == session) {
            seat = i;
            break;
        }
    }

    if (seat < 0) return;

    // 设置玩家动作
    HumanPlayer* human = dynamic_cast<HumanPlayer*>(players[seat]);
    if (human) {
        human->setAction(action);
    }
}

void Room::broadcast(const std::string& message) {
    for (int i = 0; i < 4; ++i) {
        if (sessions[i]) {
            sessions[i]->send(message);
        }
    }
}

void Room::broadcastState(int for_seat) {
    GameState state = getCurrentState();

    if (for_seat >= 0) {
        // 发送给特定玩家
        if (sessions[for_seat]) {
            GameState view = state.getViewFor(for_seat);
            GameMessage msg;
            msg.type = MessageType::GameState;
            msg.state = view;
            sessions[for_seat]->send(msg.toJSON());
        }
    } else {
        // 分别发送给每个玩家 (各自视角)
        for (int i = 0; i < 4; ++i) {
            if (sessions[i]) {
                GameState view = state.getViewFor(i);
                GameMessage msg;
                msg.type = MessageType::GameState;
                msg.state = view;
                sessions[i]->send(msg.toJSON());
            }
        }
    }
}

GameState Room::getCurrentState() const {
    GameState state;
    state.round_wind = static_cast<int>(game_table ? game_table->getRoundWind() : Wind::East);
    state.dealer = game_table ? game_table->getDealer() : 0;
    state.current_player = game_table ? game_table->getCurrentPlayer() : 0;
    state.remaining_tiles = game_table ? game_table->getRemainingTiles() : 70;
    state.honba = 0;
    state.riichi_sticks = 0;

    // 收集各家信息
    for (int i = 0; i < 4; ++i) {
        state.scores[i] = players[i] ? players[i]->getScore() : 25000;
        state.riichi_status[i] = false;  // TODO

        // 手牌和牌河需要从 Player/Hand 获取
        // 这里简化处理
        state.discards[i] = std::vector<int>(players[i] ? players[i]->getDiscards().begin() : std::vector<int>::iterator(),
                                              players[i] ? players[i]->getDiscards().end() : std::vector<int>::iterator());
    }

    state.can_tsumo = false;
    state.can_ron = false;
    state.can_riichi = false;
    state.can_chi = false;
    state.can_pon = false;
    state.can_kan = false;
    state.last_draw = -1;
    state.last_discard = -1;
    state.last_discard_player = -1;

    return state;
}
