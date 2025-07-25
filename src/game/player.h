#ifndef PLAYER_H
#define PLAYER_H

#include <vector>
#include <string>

#include "table.h"
#include "types.h"

class Player {
private:
    Hand *hand; // 1m-9m, 1p-9p, 1s-9s, 1z-7z
    Table *table;
public:
    Player();
    ~Player();

    bool canDiscard( const Tile &tile ) const;
    bool canChi( const Tile &tile ) const;
    bool canPon( const Tile &tile ) const;
    bool canKan( const Tile &tile ) const;
    bool canAnKan( const Tile &tile ) const;
    bool canWin(const Tile &tile) const;

    virtual int handleDraw( const Tile &tile );
    virtual int handleLeftPlayerDiscard( const Tile &tile );
    virtual int handleOtherPlayerDiscard( const Tile &tile );
};

/*
0 - 135: discard one tile
136    : win
137    : chi
138    : pon
139    : kan
140    : ankan
141    : do nothing
*/

#endif // PLAYER_H