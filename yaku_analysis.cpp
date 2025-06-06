#include <cassert>

#include "types.h"
#include "constants.h"
#include "tiles.h"

bool Hand::isTanyao(const Tile &draw) const{
    TileCounts counts(tile_counts);
    assert(isValidTile(draw));
    counts[getTileType(draw)]++;

    for ( TileType tile : Yao.list ){
        if ( counts[tile] > 0) return false;
    }
    
    return true;
}

bool Hand::isChiitoitsu(const Tile &draw) const{
    TileCounts counts(tile_counts);
    assert(isValidTile(draw));
    counts[getTileType(draw)]++;

    int pairs = 0;
    for (TileType tile : All.list)
        if (counts[tile] >= 2) pairs++;
    return pairs == 7;
}

bool Hand::isRyanpeikou(const Tile &draw) const{
    TileCounts counts(tile_counts);
    assert(isValidTile(draw));
    counts[getTileType(draw)]++;
    
    int sequenceCount = 0, pairCount = 0;
    for ( TileType tile : SeqBegun.list ){
        if ( counts[tile] >= 2 && counts[tile + 1] >= 2 && counts[tile + 2] >= 2 ) {
            sequenceCount++; counts[tile] -= 2; counts[tile + 1] -= 2;
            counts[tile + 2] -= 2;
        }
    }
    for ( TileType tile : All.list ){
        if ( counts[tile] >= 2 ) {
            pairCount++;
        }
    }

    return sequenceCount == 2 && pairCount == 1;
}

bool Hand::isKokushiMuso(const Tile &draw) const {
    TileCounts counts(tile_counts);
    if ( isKokushiMusoJusanmen(draw) ) return false;
    counts[getTileType(draw)]++;

    for (TileType tile : Yao.list) {
        if (counts[tile] == 0) return false;
    }
    
    return true;
}

bool Hand::isKokushiMusoJusanmen(const Tile &draw) const{
    TileCounts counts(tile_counts);
    assert(isValidTile(draw));
    if ( !Yao.contains(draw) ) return false;
    
    for (TileType tile : Yao.list) {
        if (counts[tile] == 0) return false;
    }
    
    return true;
}