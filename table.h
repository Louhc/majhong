#ifndef TABLE_H
#define TABLE_H

#include <vector>
#include <string>
#include "player.h"

class Table {
private:
    Player *players[4]; // Array of pointers to Player objects
public:
    Table();
    ~Table();
};

#endif // TABLE_H