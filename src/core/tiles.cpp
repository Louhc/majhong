#include "tiles.h"
#include "constants.h"

bool isValidTile(const Tile &tile){
    return tile >= 0 && tile < 136;
}

int getTileType(const Tile &tile){
    return tile / 4;
}

int getSuitIndex(const Tile &tile){
    return tile / 36;
}

int getTileValue(const Tile &tile){
    return tile / 4 % 9 + 1;
}

TileName getTileName(const Tile &tile){
    assert(isValidTile(tile));
    int suitIndex = getSuitIndex(tile);
    int rank = tile / 4 % 9 + 1;
    if ( Five.contains(tile) && tile % 4 == 0 ) rank = 0;
    return std::to_string((int)rank) + suits[suitIndex];
}