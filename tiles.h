#ifndef TILES_H
#define TILES_H

#include <cassert>

#include "types.h"
#include "constants.h"

bool isValidTile(Tile tile) {
    return tile >= 0 && tile < 136;
}

int getTileType(Tile tile) {
    return tile / 4;
}

int getSuitIndex(Tile tile) {
    return tile / 36;
}

int getTileValue(Tile tile) {
    return tile / 4 % 9 + 1;
}

TileName getTileName(Tile tile){
    assert(isValidTile(tile));
    int suitIndex = getSuitIndex(tile);
    int rank = tile / 4 % 9 + 1;
    if ( Five.contains(tile) && tile % 4 == 0 ) rank = 0;
    return std::to_string((int)rank) + suits[suitIndex];
}

#endif // TILES_H