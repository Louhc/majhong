#include "tiles.h"
#include "constants.h"

bool isValidTileIndex(const TileIndex &tile_index){
    return tile_index >= 0 && tile_index < 136;
}

bool isValidTile(const Tile &tile){
    return tile >= 0 && tile < 136;
}

int getSuitIndex(const TileIndex &tile_index){
    return tile_index / 36;
}

int getTileValue(const TileIndex &tile_index){
    return tile_index / 4 % 9 + 1;
}

TileName getTileName(const TileIndex &tile_index){
    assert(isValidTileIndex(tile_index));
    int suitIndex = getSuitIndex(tile_index);
    int rank = tile_index / 4 % 9 + 1;
    if ( Five.contains(tile_index) && tile_index % 4 == 0 ) rank = 0;
    return std::to_string((int)rank) + suits[suitIndex];
}