#ifndef PLAYER_H
#define PLAYER_H

#include <vector>
#include <string>
#include "types.h"

class Table;

// 玩家动作编码
enum class Action : int {
    // 0-135: 弃牌 (TileIndex)
    Win = 136,
    Chi = 137,
    Pon = 138,
    Kan = 139,
    Ankan = 140,
    Riichi = 141,
    Pass = 142
};

// 玩家基类
class Player {
protected:
    Hand* hand;
    Table* table;
    int seat;           // 座位 (0-3: 东南西北)
    int score;          // 点数
    std::string name;   // 玩家名称
    TileIndexList discards;  // 牌河

public:
    Player(const std::string& player_name = "Player");
    virtual ~Player();

    // 初始化
    void initHand(const TileIndexList& tiles, Wind round, Wind seat_wind);
    void setTable(Table* t, int seat_pos);

    // 获取信息
    Hand* getHand() const { return hand; }
    int getSeat() const { return seat; }
    int getScore() const { return score; }
    const std::string& getName() const { return name; }
    const TileIndexList& getDiscards() const { return discards; }

    // 点数操作
    void addScore(int delta) { score += delta; }

    // 判定方法 (委托给 Hand)
    bool canChi(TileIndex tile) const;
    bool canPon(TileIndex tile) const;
    bool canKan(TileIndex tile) const;
    bool canAnkan() const;
    bool canWin(TileIndex tile) const;
    bool canTsumo() const;  // 自摸

    // 执行动作
    void draw(TileIndex tile);               // 摸牌
    void discard(TileIndex tile);            // 弃牌
    void callChi(TileIndex call, TileIndex tile1, TileIndex tile2);
    void callPon(TileIndex call);
    void callKan(TileIndex call);
    void performAnkan(TileIndex tile);

    // 决策接口 (子类实现)
    // 返回: 0-135 弃牌, 或 Action 枚举值
    virtual int decideAction(TileIndex drawn_tile, bool can_tsumo, bool can_ankan, bool can_riichi) = 0;
    virtual int decideResponse(TileIndex discard, int from_seat, bool can_chi, bool can_pon, bool can_kan, bool can_ron) = 0;

    // 如果选择吃，返回要打出的两张牌
    virtual std::pair<TileIndex, TileIndex> selectChiTiles(TileIndex call) { return {-1, -1}; }
};

#endif // PLAYER_H
