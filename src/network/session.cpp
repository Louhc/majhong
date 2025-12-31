#include "session.h"

Session::Session(int session_id)
    : id(session_id), current_room(nullptr), seat(-1) {
}

void Session::setRoom(Room* room, int seat_pos) {
    current_room = room;
    seat = seat_pos;
}

void Session::send(const std::string& message) {
    if (send_callback) {
        send_callback(message);
    }
}
