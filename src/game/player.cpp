#include "player.h"
#include "table.h"

Player::Player(const std::string& player_name)
    : hand(nullptr), table(nullptr), seat(-1), score(25000), name(player_name) {
}

Player::~Player() {
    delete hand;
}

void Player::initHand(const TileIndexList& tiles, Wind round, Wind seat_wind) {
    delete hand;
    hand = new Hand(tiles, round, seat_wind);
}

void Player::setTable(Table* t, int seat_pos) {
    table = t;
    seat = seat_pos;
}

// 判定方法
bool Player::canChi(TileIndex tile) const {
    if (!hand) return false;
    return hand->canChi(tile);
}

bool Player::canPon(TileIndex tile) const {
    if (!hand) return false;
    return hand->canPon(tile);
}

bool Player::canKan(TileIndex tile) const {
    if (!hand) return false;
    return hand->canKan(tile);
}

bool Player::canAnkan() const {
    if (!hand) return false;
    TileCounts counts = hand->getTileCounts();
    for (int i = 0; i < 34; ++i) {
        if (counts[i] == 4) return true;
    }
    return false;
}

bool Player::canWin(TileIndex tile) const {
    if (!hand) return false;
    return hand->isWinningHand(tile);
}

bool Player::canTsumo() const {
    // 自摸检查: 需要在摸牌后检查 (14张状态)
    // 这里假设已经摸牌，检查是否能和
    return false;  // 由 Table 处理
}

// 执行动作
void Player::draw(TileIndex tile) {
    // 摸牌由 Hand 内部管理，这里只更新状态
    // Hand 目前没有单独的 draw 方法，需要通过 drawAndDiscard
}

void Player::discard(TileIndex tile) {
    discards.push_back(tile);
    // Hand 的弃牌在 drawAndDiscard 中处理
}

void Player::callChi(TileIndex call, TileIndex tile1, TileIndex tile2) {
    if (hand) {
        hand->callChi(call, tile1, 0);  // opt 参数需要根据实际情况调整
    }
}

void Player::callPon(TileIndex call) {
    if (hand) {
        // callPon 需要 discard 参数，这里传 0 作为占位
        // 实际实现中应该由 Table 管理
    }
}

void Player::callKan(TileIndex call) {
    if (hand) {
        hand->callKan(call);
    }
}

void Player::performAnkan(TileIndex tile) {
    if (hand) {
        hand->performAnkan(tile);
    }
}
