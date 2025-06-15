#ifndef TABLE_H
#define TABLE_H

#include <vector>
#include <string>
#include "types.h"

class Player;

class Table {
private:
    Player *players[4];
    int current_player;
    TileList wall;
    int wall_pointer;
    int kan_counts;
    bool is_started;

public:
    Table();
    ~Table();

    int getPosition( const Player *who ) const;

    void start();
};

#endif // TABLE_H