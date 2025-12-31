#include <iostream>
#include "types.h"
#include "table.h"
#include "simple_ai.h"
#include "printer.h"

int main() {
    std::cout << "=== 麻将游戏测试 ===" << std::endl;

    // 创建牌桌
    Table table;

    // 创建4个AI玩家
    SimpleAI* ai1 = new SimpleAI("AI-东");
    SimpleAI* ai2 = new SimpleAI("AI-南");
    SimpleAI* ai3 = new SimpleAI("AI-西");
    SimpleAI* ai4 = new SimpleAI("AI-北");

    table.setPlayer(0, ai1);
    table.setPlayer(1, ai2);
    table.setPlayer(2, ai3);
    table.setPlayer(3, ai4);

    // 设置回调来观察游戏进程
    GameCallbacks callbacks;
    callbacks.onTurnStart = [](int seat) {
        std::cout << "玩家 " << seat << " 的回合" << std::endl;
    };
    callbacks.onDraw = [](int seat, TileIndex tile) {
        std::cout << "  玩家 " << seat << " 摸牌: ";
        printTileIndex(tile);
        std::cout << std::endl;
    };
    callbacks.onDiscard = [](int seat, TileIndex tile) {
        std::cout << "  玩家 " << seat << " 弃牌: ";
        printTileIndex(tile);
        std::cout << std::endl;
    };
    callbacks.onMeld = [](int seat, int action, TileIndex tile) {
        std::cout << "  玩家 " << seat << " 副露动作 " << action << std::endl;
    };
    callbacks.onGameEnd = [](const GameResult& result) {
        std::cout << "\n=== 游戏结束 ===" << std::endl;
        if (result.winner >= 0) {
            std::cout << "赢家: 玩家 " << result.winner;
            if (result.is_tsumo) {
                std::cout << " (自摸)";
            } else {
                std::cout << " (荣和)";
            }
            std::cout << std::endl;
        } else {
            std::cout << "流局" << std::endl;
        }
    };

    table.setCallbacks(callbacks);

    // 运行一局游戏
    std::cout << "\n开始游戏...\n" << std::endl;
    GameResult result = table.playRound();

    // 清理
    delete ai1;
    delete ai2;
    delete ai3;
    delete ai4;

    return 0;
}
