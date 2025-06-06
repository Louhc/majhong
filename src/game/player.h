#ifndef PLAYER_H
#define PLAYER_H

#include <vector>
#include <string>

#include "table.h"
#include "types.h"

class Player {
private:
    Hand hand; // 1m-9m, 1p-9p, 1s-9s, 1z-7z
    Table *table;
public:
    Player();
    ~Player();
};

#endif // PLAYER_H