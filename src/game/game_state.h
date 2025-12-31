#ifndef GAME_STATE_H
#define GAME_STATE_H

#include <vector>
#include <array>
#include <string>
#include "types.h"

// 游戏状态 (用于网络同步和状态保存)
struct GameState {
    // 游戏基本信息
    int round_wind;           // 场风 (0-3: 东南西北)
    int dealer;               // 庄家座位
    int current_player;       // 当前玩家
    int remaining_tiles;      // 剩余牌数
    int honba;                // 本场数
    int riichi_sticks;        // 场上立直棒

    // 各家手牌 (对于请求者可见的部分)
    std::array<std::vector<int>, 4> hands;

    // 各家牌河
    std::array<std::vector<int>, 4> discards;

    // 各家副露
    std::array<std::vector<std::pair<int, std::vector<int>>>, 4> melds;  // (类型, 牌)

    // 各家点数
    std::array<int, 4> scores;

    // 各家立直状态
    std::array<bool, 4> riichi_status;

    // 当前可用动作 (针对请求玩家)
    bool can_tsumo;
    bool can_ron;
    bool can_riichi;
    bool can_chi;
    bool can_pon;
    bool can_kan;

    // 最后摸到/打出的牌
    int last_draw;
    int last_discard;
    int last_discard_player;

    // 序列化为 JSON 字符串
    std::string toJSON() const;

    // 从 JSON 字符串反序列化
    static GameState fromJSON(const std::string& json);

    // 创建针对特定玩家的视角 (隐藏其他玩家手牌)
    GameState getViewFor(int seat) const;
};

// 游戏消息类型
enum class MessageType {
    // 客户端 -> 服务器
    CreateRoom,
    JoinRoom,
    LeaveRoom,
    Ready,
    Action,

    // 服务器 -> 客户端
    RoomCreated,
    RoomJoined,
    PlayerJoined,
    PlayerLeft,
    GameStart,
    GameState,
    YourTurn,
    PlayerAction,
    GameEnd,
    Error
};

// 游戏消息
struct GameMessage {
    MessageType type;
    std::string room_id;
    int seat;
    int action;
    int tile;
    std::vector<int> tiles;
    std::string error_msg;
    GameState state;

    std::string toJSON() const;
    static GameMessage fromJSON(const std::string& json);
};

#endif // GAME_STATE_H
