#include "types.h"

TileMap getTileMap( const TileList& list ) {
    TileMap map; map.fill(false);
    for ( const Tile& tile : list ) 
        map[tile] = true;
    return map;
}

bool TileFamily::contains(const Tile &tile) const{
    return map[tile];
}

bool TileFamily::containsIdx(const TileIndex &tile_index) const{
    return map[tile_index / 4];
}