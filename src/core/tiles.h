#ifndef TILES_H
#define TILES_H

#include <cassert>
#include "types.h"

bool isValidTileIndex(const TileIndex &tile_index);

bool isValidTile(const Tile &tile);

int getSuitIndex(const TileIndex &tile_index);

int getTileValue(const TileIndex &tile_index);

TileName getTileName(const TileIndex &tile_index);

#endif // TILES_H