#include "server.h"
#include "game_state.h"
#include <sstream>
#include <iostream>

GameServer::GameServer(int p)
    : port(p), running(false), next_session_id(1), next_room_id(1) {
}

void GameServer::start() {
    running = true;
    std::cout << "Game server started on port " << port << std::endl;

    // 注意: 实际的 WebSocket 服务器需要使用具体的库实现
    // 这里只是框架代码
}

void GameServer::stop() {
    running = false;
    std::cout << "Game server stopped" << std::endl;
}

Session* GameServer::createSession() {
    int id = next_session_id++;
    auto session = std::make_unique<Session>(id);
    Session* ptr = session.get();
    sessions[id] = std::move(session);
    return ptr;
}

void GameServer::removeSession(int session_id) {
    auto it = sessions.find(session_id);
    if (it != sessions.end()) {
        Session* session = it->second.get();

        // 从房间中移除
        Room* room = session->getRoom();
        if (room) {
            room->removePlayer(session);
        }

        sessions.erase(it);
    }
}

Room* GameServer::createRoom() {
    std::ostringstream ss;
    ss << "ROOM" << next_room_id++;
    std::string room_id = ss.str();

    auto room = std::make_unique<Room>(room_id);
    Room* ptr = room.get();
    rooms[room_id] = std::move(room);

    std::cout << "Room created: " << room_id << std::endl;
    return ptr;
}

Room* GameServer::getRoom(const std::string& room_id) {
    auto it = rooms.find(room_id);
    if (it != rooms.end()) {
        return it->second.get();
    }
    return nullptr;
}

void GameServer::removeRoom(const std::string& room_id) {
    rooms.erase(room_id);
    std::cout << "Room removed: " << room_id << std::endl;
}

std::vector<std::string> GameServer::listRooms() const {
    std::vector<std::string> result;
    for (const auto& pair : rooms) {
        result.push_back(pair.first);
    }
    return result;
}

void GameServer::handleMessage(Session* session, const std::string& message) {
    // 解析消息
    GameMessage msg = GameMessage::fromJSON(message);

    switch (msg.type) {
        case MessageType::CreateRoom: {
            Room* room = createRoom();
            if (room && room->addPlayer(session)) {
                GameMessage response;
                response.type = MessageType::RoomCreated;
                response.room_id = room->getId();
                response.seat = session->getSeat();
                session->send(response.toJSON());
            }
            break;
        }

        case MessageType::JoinRoom: {
            Room* room = getRoom(msg.room_id);
            if (room && room->addPlayer(session)) {
                // 通知加入成功
                GameMessage response;
                response.type = MessageType::RoomJoined;
                response.room_id = room->getId();
                response.seat = session->getSeat();
                session->send(response.toJSON());

                // 通知房间内其他玩家
                GameMessage notify;
                notify.type = MessageType::PlayerJoined;
                notify.seat = session->getSeat();
                room->broadcast(notify.toJSON());
            } else {
                GameMessage error;
                error.type = MessageType::Error;
                error.error_msg = "Failed to join room";
                session->send(error.toJSON());
            }
            break;
        }

        case MessageType::LeaveRoom: {
            Room* room = session->getRoom();
            if (room) {
                int seat = session->getSeat();
                room->removePlayer(session);

                // 通知其他玩家
                GameMessage notify;
                notify.type = MessageType::PlayerLeft;
                notify.seat = seat;
                room->broadcast(notify.toJSON());
            }
            break;
        }

        case MessageType::Ready: {
            Room* room = session->getRoom();
            if (room && room->getState() == RoomState::Waiting) {
                // 检查是否所有人都准备好了
                // 简化: 收到 Ready 就开始游戏
                room->startGame();
            }
            break;
        }

        case MessageType::Action: {
            Room* room = session->getRoom();
            if (room && room->getState() == RoomState::Playing) {
                room->handleAction(session, msg.action, msg.tile);
            }
            break;
        }

        default:
            break;
    }
}

void GameServer::onClientConnect(int client_fd) {
    Session* session = createSession();

    // 设置发送回调
    session->setSendCallback([client_fd](const std::string& msg) {
        // 实际的发送需要使用具体的网络库
        std::cout << "Send to client " << client_fd << ": " << msg << std::endl;
    });

    if (on_connect) {
        on_connect(session);
    }
}

void GameServer::onClientMessage(int client_fd, const std::string& message) {
    // 查找对应的 session
    // 实际实现需要维护 client_fd -> session_id 的映射
    // 这里简化处理
}

void GameServer::onClientDisconnect(int client_fd) {
    // 实际实现需要维护 client_fd -> session_id 的映射
}
