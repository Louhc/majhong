#ifndef ROOM_H
#define ROOM_H

#include <string>
#include <vector>
#include <array>
#include <memory>
#include "session.h"
#include "table.h"
#include "simple_ai.h"
#include "game_state.h"

// 房间状态
enum class RoomState {
    Waiting,    // 等待玩家加入
    Playing,    // 游戏中
    Finished    // 游戏结束
};

// 人类玩家 (通过网络控制)
class HumanPlayer : public Player {
private:
    Session* session;
    int pending_action;
    bool action_ready;

public:
    HumanPlayer(Session* s, const std::string& name);

    Session* getSession() const { return session; }

    // 设置玩家的决策 (由网络消息触发)
    void setAction(int action);
    bool isActionReady() const { return action_ready; }
    void clearAction() { action_ready = false; }

    // 实现决策接口 (阻塞等待网络输入)
    int decideAction(TileIndex drawn_tile, bool can_tsumo, bool can_ankan, bool can_riichi) override;
    int decideResponse(TileIndex discard, int from_seat, bool can_chi, bool can_pon, bool can_kan, bool can_ron) override;
};

// 游戏房间
class Room {
private:
    std::string room_id;
    std::array<Session*, 4> sessions;  // 玩家会话 (nullptr 表示 AI 或空位)
    std::array<Player*, 4> players;    // 玩家对象
    Table* game_table;
    RoomState state;
    int player_count;

public:
    Room(const std::string& id);
    ~Room();

    // 房间信息
    const std::string& getId() const { return room_id; }
    RoomState getState() const { return state; }
    int getPlayerCount() const { return player_count; }

    // 玩家管理
    bool addPlayer(Session* session);       // 添加人类玩家
    void removePlayer(Session* session);    // 移除玩家
    void fillWithAI();                      // 用 AI 填充空位

    // 游戏控制
    void startGame();                       // 开始游戏
    void handleAction(Session* session, int action, int tile = -1);  // 处理玩家动作

    // 广播消息
    void broadcast(const std::string& message);
    void broadcastState(int for_seat = -1);  // 广播游戏状态

    // 获取当前游戏状态
    GameState getCurrentState() const;

private:
    int findEmptySeat() const;
    void setupCallbacks();
};

#endif // ROOM_H
