#ifndef SIMPLE_AI_H
#define SIMPLE_AI_H

#include "player.h"
#include <random>

// 简单 AI 玩家
// 策略:
// 1. 能和则和
// 2. 打字牌优先 (非役牌)
// 3. 打边张/孤张
// 4. 随机选择
class SimpleAI : public Player {
private:
    std::mt19937 rng;
    TileIndex last_drawn;  // 上次摸到的牌

public:
    SimpleAI(const std::string& name = "AI");

    // 实现决策接口
    int decideAction(TileIndex drawn_tile, bool can_tsumo, bool can_ankan, bool can_riichi) override;
    int decideResponse(TileIndex discard, int from_seat, bool can_chi, bool can_pon, bool can_kan, bool can_ron) override;

private:
    // AI 策略方法
    TileIndex selectDiscard();           // 选择要打出的牌
    int evaluateTile(Tile tile) const;   // 评估牌的价值 (越低越应该打出)
    bool shouldPon(TileIndex tile) const; // 是否应该碰
    bool shouldChi(TileIndex tile) const; // 是否应该吃
};

#endif // SIMPLE_AI_H
