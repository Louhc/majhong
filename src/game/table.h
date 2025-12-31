#ifndef TABLE_H
#define TABLE_H

#include <vector>
#include <array>
#include <string>
#include <functional>
#include <random>
#include "types.h"

class Player;

// 游戏结果
struct GameResult {
    int winner;           // 赢家座位 (-1 表示流局)
    bool is_tsumo;        // 是否自摸
    int from_player;      // 点炮者 (-1 表示自摸)
    YakuList yaku;        // 役种
    int han;              // 番数
    int fu;               // 符数
    int score;            // 得点
};

// 游戏事件回调 (用于网络同步)
struct GameCallbacks {
    std::function<void(int seat, TileIndex tile)> onDraw;
    std::function<void(int seat, TileIndex tile)> onDiscard;
    std::function<void(int seat, int action, TileIndex tile)> onMeld;
    std::function<void(const GameResult&)> onGameEnd;
    std::function<void(int seat)> onTurnStart;
};

class Table {
private:
    std::array<Player*, 4> players;
    int current_player;       // 当前玩家 (0-3)
    int dealer;               // 庄家 (0-3)
    Wind round_wind;          // 场风

    TileIndexList wall;       // 牌山 (136张)
    int wall_pointer;         // 牌山指针
    int dead_wall_start;      // 王牌起始位置

    int kan_count;            // 杠次数
    int honba;                // 本场数
    int riichi_sticks;        // 立直棒数

    bool is_started;
    bool is_finished;

    GameCallbacks callbacks;
    std::mt19937 rng;

public:
    Table();
    ~Table();

    // 设置玩家
    void setPlayer(int seat, Player* player);
    Player* getPlayer(int seat) const { return players[seat]; }

    // 设置回调
    void setCallbacks(const GameCallbacks& cb) { callbacks = cb; }

    // 游戏信息
    int getCurrentPlayer() const { return current_player; }
    int getDealer() const { return dealer; }
    Wind getRoundWind() const { return round_wind; }
    int getRemainingTiles() const { return dead_wall_start - wall_pointer; }
    bool isFinished() const { return is_finished; }

    // 游戏流程
    void initRound();         // 初始化一局
    void shuffleWall();       // 洗牌
    void dealTiles();         // 发牌
    GameResult playRound();   // 运行一局游戏

    // 单步执行 (用于网络同步)
    TileIndex drawTile();                    // 摸一张牌
    void processDiscard(TileIndex tile);     // 处理弃牌
    int checkResponses(TileIndex discard, int from_seat);  // 检查其他玩家响应

private:
    Wind getSeatWind(int seat) const;
    void nextPlayer();
    bool isWallEmpty() const;
    TileIndex drawFromDeadWall();  // 岭上摸牌
};

#endif // TABLE_H
