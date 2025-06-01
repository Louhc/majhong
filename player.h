#ifndef PLAYER_H
#define PLAYER_H

#include <vector>
#include <string>
#include "table.h"

int nextStep( std::vector<char> &hand, char tile) {
    // This function should implement the logic to determine the next step
    // based on the player's hand and the tile drawn.
    // For now, we will return 0 as a placeholder.
    return 0;
}

class Player {
private:
    std::vector<char> hand; // 1m-9m, 1p-9p, 1s-9s, 1z-7z
    Table *table;
public:
    void drawTile(char tile) {
        hand.push_back(tile);
    }
    Player();
    ~Player();
};

#endif // PLAYER_H