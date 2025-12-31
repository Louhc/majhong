#ifndef SERVER_H
#define SERVER_H

#include <string>
#include <map>
#include <memory>
#include <functional>
#include "session.h"
#include "room.h"

// 游戏服务器
// 注意: 这是一个抽象接口，实际的 WebSocket 实现需要使用具体库
class GameServer {
public:
    using ConnectionCallback = std::function<void(Session*)>;
    using MessageCallback = std::function<void(Session*, const std::string&)>;
    using DisconnectCallback = std::function<void(Session*)>;

private:
    int port;
    bool running;
    int next_session_id;
    int next_room_id;

    std::map<int, std::unique_ptr<Session>> sessions;
    std::map<std::string, std::unique_ptr<Room>> rooms;

    ConnectionCallback on_connect;
    MessageCallback on_message;
    DisconnectCallback on_disconnect;

public:
    GameServer(int port = 8080);
    virtual ~GameServer() = default;

    // 服务器控制
    virtual void start();
    virtual void stop();
    bool isRunning() const { return running; }

    // 回调设置
    void setOnConnect(ConnectionCallback cb) { on_connect = cb; }
    void setOnMessage(MessageCallback cb) { on_message = cb; }
    void setOnDisconnect(DisconnectCallback cb) { on_disconnect = cb; }

    // 房间管理
    Room* createRoom();
    Room* getRoom(const std::string& room_id);
    void removeRoom(const std::string& room_id);
    std::vector<std::string> listRooms() const;

    // 消息处理
    void handleMessage(Session* session, const std::string& message);

protected:
    // 子类实现实际的网络操作
    virtual void onClientConnect(int client_fd);
    virtual void onClientMessage(int client_fd, const std::string& message);
    virtual void onClientDisconnect(int client_fd);

    Session* createSession();
    void removeSession(int session_id);
};

#endif // SERVER_H
