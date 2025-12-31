#ifndef SESSION_H
#define SESSION_H

#include <string>
#include <functional>

class Room;

// 玩家会话 (表示一个网络连接)
class Session {
public:
    using MessageCallback = std::function<void(const std::string&)>;

private:
    int id;
    std::string player_name;
    Room* current_room;
    int seat;  // 在房间中的座位 (-1 表示观战)
    MessageCallback send_callback;

public:
    Session(int session_id);
    virtual ~Session() = default;

    // 基本信息
    int getId() const { return id; }
    const std::string& getName() const { return player_name; }
    void setName(const std::string& name) { player_name = name; }

    // 房间关联
    Room* getRoom() const { return current_room; }
    void setRoom(Room* room, int seat_pos = -1);
    int getSeat() const { return seat; }

    // 消息发送
    void setSendCallback(MessageCallback cb) { send_callback = cb; }
    void send(const std::string& message);
};

#endif // SESSION_H
