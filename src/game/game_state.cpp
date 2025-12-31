#include "game_state.h"
#include <sstream>

// 简单的 JSON 序列化实现
// 注意: 生产环境应使用 nlohmann/json 等库

static std::string escapeJSON(const std::string& s) {
    std::ostringstream ss;
    for (char c : s) {
        switch (c) {
            case '"': ss << "\\\""; break;
            case '\\': ss << "\\\\"; break;
            case '\n': ss << "\\n"; break;
            case '\r': ss << "\\r"; break;
            case '\t': ss << "\\t"; break;
            default: ss << c;
        }
    }
    return ss.str();
}

static std::string vectorToJSON(const std::vector<int>& v) {
    std::ostringstream ss;
    ss << "[";
    for (size_t i = 0; i < v.size(); ++i) {
        if (i > 0) ss << ",";
        ss << v[i];
    }
    ss << "]";
    return ss.str();
}

std::string GameState::toJSON() const {
    std::ostringstream ss;
    ss << "{";

    // 基本信息
    ss << "\"round_wind\":" << round_wind << ",";
    ss << "\"dealer\":" << dealer << ",";
    ss << "\"current_player\":" << current_player << ",";
    ss << "\"remaining_tiles\":" << remaining_tiles << ",";
    ss << "\"honba\":" << honba << ",";
    ss << "\"riichi_sticks\":" << riichi_sticks << ",";

    // 手牌
    ss << "\"hands\":[";
    for (int i = 0; i < 4; ++i) {
        if (i > 0) ss << ",";
        ss << vectorToJSON(hands[i]);
    }
    ss << "],";

    // 牌河
    ss << "\"discards\":[";
    for (int i = 0; i < 4; ++i) {
        if (i > 0) ss << ",";
        ss << vectorToJSON(discards[i]);
    }
    ss << "],";

    // 点数
    ss << "\"scores\":[";
    for (int i = 0; i < 4; ++i) {
        if (i > 0) ss << ",";
        ss << scores[i];
    }
    ss << "],";

    // 立直状态
    ss << "\"riichi_status\":[";
    for (int i = 0; i < 4; ++i) {
        if (i > 0) ss << ",";
        ss << (riichi_status[i] ? "true" : "false");
    }
    ss << "],";

    // 可用动作
    ss << "\"can_tsumo\":" << (can_tsumo ? "true" : "false") << ",";
    ss << "\"can_ron\":" << (can_ron ? "true" : "false") << ",";
    ss << "\"can_riichi\":" << (can_riichi ? "true" : "false") << ",";
    ss << "\"can_chi\":" << (can_chi ? "true" : "false") << ",";
    ss << "\"can_pon\":" << (can_pon ? "true" : "false") << ",";
    ss << "\"can_kan\":" << (can_kan ? "true" : "false") << ",";

    // 最后的牌
    ss << "\"last_draw\":" << last_draw << ",";
    ss << "\"last_discard\":" << last_discard << ",";
    ss << "\"last_discard_player\":" << last_discard_player;

    ss << "}";
    return ss.str();
}

GameState GameState::fromJSON(const std::string& json) {
    // TODO: 实现 JSON 解析
    // 生产环境应使用 nlohmann/json
    GameState state;
    return state;
}

GameState GameState::getViewFor(int seat) const {
    GameState view = *this;

    // 隐藏其他玩家的手牌 (用 -1 表示暗牌)
    for (int i = 0; i < 4; ++i) {
        if (i != seat) {
            size_t count = view.hands[i].size();
            view.hands[i].clear();
            view.hands[i].resize(count, -1);  // 只显示牌数，不显示具体牌
        }
    }

    return view;
}

std::string GameMessage::toJSON() const {
    std::ostringstream ss;
    ss << "{";

    ss << "\"type\":" << static_cast<int>(type) << ",";
    ss << "\"room_id\":\"" << escapeJSON(room_id) << "\",";
    ss << "\"seat\":" << seat << ",";
    ss << "\"action\":" << action << ",";
    ss << "\"tile\":" << tile << ",";
    ss << "\"tiles\":" << vectorToJSON(tiles) << ",";
    ss << "\"error_msg\":\"" << escapeJSON(error_msg) << "\",";
    ss << "\"state\":" << state.toJSON();

    ss << "}";
    return ss.str();
}

GameMessage GameMessage::fromJSON(const std::string& json) {
    // TODO: 实现 JSON 解析
    GameMessage msg;
    msg.type = MessageType::Error;
    return msg;
}
