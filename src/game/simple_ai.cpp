#include "simple_ai.h"
#include "constants.h"
#include <chrono>
#include <algorithm>

SimpleAI::SimpleAI(const std::string& name) : Player(name), last_drawn(-1) {
    auto seed = std::chrono::steady_clock::now().time_since_epoch().count();
    rng.seed(static_cast<unsigned>(seed));
}

int SimpleAI::decideAction(TileIndex drawn_tile, bool can_tsumo, bool can_ankan, bool can_riichi) {
    last_drawn = drawn_tile;

    // 1. 能和则和
    if (can_tsumo) {
        return static_cast<int>(Action::Win);
    }

    // 2. 暂时不处理暗杠 (简单 AI)
    // if (can_ankan) { ... }

    // 3. 选择弃牌
    TileIndex discard = selectDiscard();
    return discard;
}

int SimpleAI::decideResponse(TileIndex discard, int from_seat, bool can_chi, bool can_pon, bool can_kan, bool can_ron) {
    // 1. 能荣和则荣和
    if (can_ron) {
        return static_cast<int>(Action::Win);
    }

    // 2. 考虑碰 (役牌优先)
    if (can_pon && shouldPon(discard)) {
        return static_cast<int>(Action::Pon);
    }

    // 3. 简单 AI 一般不吃 (吃容易破坏牌型)
    // if (can_chi && shouldChi(discard)) {
    //     return static_cast<int>(Action::Chi);
    // }

    // 4. 不响应
    return static_cast<int>(Action::Pass);
}

TileIndex SimpleAI::selectDiscard() {
    if (!hand) return last_drawn;

    TileCounts counts = hand->getTileCounts();

    // 收集所有可打的牌及其评估值
    std::vector<std::pair<int, Tile>> candidates;  // (评估值, 牌)

    for (int tile = 0; tile < 34; ++tile) {
        if (counts[tile] > 0) {
            int value = evaluateTile(tile);
            candidates.push_back({value, tile});
        }
    }

    // 按评估值排序 (值越低越应该打出)
    std::sort(candidates.begin(), candidates.end());

    if (candidates.empty()) {
        return last_drawn;
    }

    // 选择评估值最低的牌
    Tile best_tile = candidates[0].second;

    // 找到对应的 TileIndex
    // 遍历手牌找到这张牌的 TileIndex
    // 由于 Hand 类没有暴露手牌列表，我们用 last_drawn 或随机选择
    // 简化处理: 返回 tile * 4 作为 TileIndex
    return best_tile * 4;
}

int SimpleAI::evaluateTile(Tile tile) const {
    // 评估牌的价值 (值越低越应该打出)
    // 返回 0-100 的值

    if (!hand) return 50;

    TileCounts counts = hand->getTileCounts();
    int value = 50;

    // 字牌 (非役牌) 优先打出
    if (Honor.contains(tile)) {
        // 检查是否是役牌
        bool is_yakuhai = (tile == Haku || tile == Hatsu || tile == Chun);
        // 风牌需要检查是否是自风/场风
        // 简化: 字牌单张优先打出
        if (counts[tile] == 1) {
            value = is_yakuhai ? 30 : 10;  // 非役牌字牌单张最优先
        } else if (counts[tile] == 2) {
            value = is_yakuhai ? 60 : 40;  // 对子保留
        } else {
            value = 80;  // 刻子保留
        }
    }
    // 数牌
    else {
        int num = tile % 9 + 1;  // 1-9

        // 边张 (1, 9) 优先打出
        if (num == 1 || num == 9) {
            if (counts[tile] == 1) {
                value = 20;
            } else if (counts[tile] == 2) {
                value = 50;
            } else {
                value = 70;
            }
        }
        // 2, 8 次优先
        else if (num == 2 || num == 8) {
            if (counts[tile] == 1) {
                value = 35;
            } else {
                value = 55;
            }
        }
        // 中张 (3-7) 保留
        else {
            if (counts[tile] == 1) {
                value = 45;
            } else {
                value = 65;
            }
        }

        // 检查是否有相邻牌 (形成搭子)
        if (tile % 9 > 0 && counts[tile - 1] > 0) {
            value += 15;  // 有左邻牌，增加保留价值
        }
        if (tile % 9 < 8 && counts[tile + 1] > 0) {
            value += 15;  // 有右邻牌
        }
    }

    return value;
}

bool SimpleAI::shouldPon(TileIndex tile_index) const {
    Tile tile = tile_index / 4;

    // 役牌 (三元牌) 必碰
    if (tile == Haku || tile == Hatsu || tile == Chun) {
        return true;
    }

    // 风牌: 简化处理，也碰
    if (Kaze.contains(tile)) {
        return true;
    }

    // 其他牌: 简单 AI 不碰 (保持门清)
    return false;
}

bool SimpleAI::shouldChi(TileIndex tile_index) const {
    // 简单 AI 一般不吃
    return false;
}
