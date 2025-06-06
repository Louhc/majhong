#ifndef TILES_H
#define TILES_H

#include <cassert>
#include "types.h"

bool isValidTile(const Tile &tile);
int getTileType(const Tile &tile);
int getSuitIndex(const Tile &tile);
int getTileValue(const Tile &tile);
TileName getTileName(const Tile &tile);

#endif // TILES_H