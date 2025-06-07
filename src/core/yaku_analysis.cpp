#include <cassert>
#include <algorithm>

#include "types.h"
#include "constants.h"
#include "tiles.h"

bool backtrackParse( TileCounts current_counts, TileMeldList current_melds,
        HandParseResult& all_results, TileType draw, bool is_pair_found, int enumerator = 0 ){
    int sum = 0;
    for ( TileType tile : All.list )
        sum += current_counts[tile];
    if ( sum == 0 ){
        all_results.push_back(current_melds);
        return true;
    }
    bool flag = false;
    for ( TileType tile : All.list ){
        if ( current_counts[tile] == 0 ) continue;

        // Check for sequences
        if ( tile * 3 >= enumerator && SeqBegun.contains(tile) && current_counts[tile + 1] > 0 && current_counts[tile + 2] > 0 ){
            current_counts[tile]--; current_counts[tile + 1]--; current_counts[tile + 2]--;
            current_melds.push_back(TileMeld(MeldType::ClosedSequence, tile));
            if ( backtrackParse(current_counts, current_melds, all_results, draw, is_pair_found, tile * 3) )
                flag =  true;
            current_melds.pop_back();
            current_counts[tile]++; current_counts[tile + 1]++; current_counts[tile + 2]++;
        }

        // Check for pairs
        if ( tile * 3 + 1 >= enumerator && !is_pair_found && current_counts[tile] >= 2 ){
            current_counts[tile] -= 2;
            current_melds.push_back(TileMeld(MeldType::Pair, tile));
            std::swap(current_melds[0], current_melds.back());
            if ( backtrackParse(current_counts, current_melds, all_results, draw, true, tile * 3 + 1) )
                flag = true;
            std::swap(current_melds[0], current_melds.back());
            current_melds.pop_back();
            current_counts[tile] += 2;
        }

        // Check for triplets
        if ( tile * 3 + 2 >= enumerator && current_counts[tile] >= 3 ){
            current_counts[tile] -= 3;
            current_melds.push_back(TileMeld(MeldType::ClosedTriplet, tile));
            if ( backtrackParse(current_counts, current_melds, all_results, draw, is_pair_found, tile * 3 + 2) )
                flag = true;
            current_melds.pop_back();
            current_counts[tile] += 3;
        }
    }
    return flag;
}

HandParseResult Hand::parseWinningHand(const Tile &draw){
    HandParseResult res;
    TileCounts counts(tile_counts);
    assert(isValidTile(draw));
    counts[getTileType(draw)]++;

    backtrackParse(counts, {}, res, getTileType(draw), false);
    return res;
}

bool Hand::isTanyao(const Tile &draw) const{
    TileCounts counts(tile_counts);
    assert(isValidTile(draw));
    counts[getTileType(draw)]++;

    for ( TileType tile : Yao.list ){
        if ( counts[tile] > 0) return false;
    }
    
    return true;
}

TileType getTileFromWind(const Wind &wind) {
    switch (wind) {
        case Wind::East: return EastWind;
        case Wind::South: return SouthWind;
        case Wind::West: return WestWind;
        case Wind::North: return NorthWind;
        default: return invalid_tile_type; // Invalid wind
    }
}

bool Hand::isYakuhaiSelfWind(const Tile &draw) const{
    TileCounts counts(getAllTileCounts());
    assert(isValidTile(draw));
    counts[getTileType(draw)]++;
    
    return counts[getTileFromWind(seat_wind)] >= 3;
}

bool Hand::isYakuhaiRoundWind(const Tile &draw) const{
    TileCounts counts(getAllTileCounts());
    assert(isValidTile(draw));
    counts[getTileType(draw)]++;
    
    return counts[getTileFromWind(round_wind)] >= 3;
}

bool Hand::isYakuhaiHaku(const Tile &draw) const{
    TileCounts counts(getAllTileCounts());
    assert(isValidTile(draw));
    counts[getTileType(draw)]++;
    
    return counts[Haku] >= 3;
}

bool Hand::isYakuhaiHatsu(const Tile &draw) const{
    TileCounts counts(getAllTileCounts());
    assert(isValidTile(draw));
    counts[getTileType(draw)]++;
    
    return counts[Hatsu] >= 3;
}

bool Hand::isYakuhaiChun(const Tile &draw) const{
    TileCounts counts(getAllTileCounts());
    assert(isValidTile(draw));
    counts[getTileType(draw)]++;
    
    return counts[Chun] >= 3;
}

// Uncomplete
bool Hand::isPinfu(const Tile &draw) const{
    if ( !isMenzen() ) return false; // Must be a closed hand

    TileCounts counts(tile_counts);
    assert(isValidTile(draw));
    counts[getTileType(draw)]++;

    if (counts[getTileFromWind(seat_wind)] > 0 || counts[getTileFromWind(round_wind)] > 0 ||
        counts[Haku] > 0 || counts[Hatsu] > 0 || counts[Chun] > 0) {
        return false; // Contains honor tiles
    }

    int sequences = 0; bool double_listen = false;
    for (TileType tile : SeqBegun.list) {
        if ( counts[tile] >= 1 ) {
            if ( counts[tile + 1] < counts[tile] || counts[tile + 2] < counts[tile] ) {
                if ( counts[tile] < 2 ) return false;
                counts[tile] -= 2;
                if ( counts[tile] == 0 ) continue;
            }
            if ( counts[tile + 1] >= counts[tile] && counts[tile + 2] >= counts[tile] ) {
                sequences += counts[tile];
                counts[tile + 1] -= counts[tile];
                counts[tile + 2] -= counts[tile];
                counts[tile] = 0;
            } else {
                return false; // Not a valid sequence
            }
        }
    }

    return sequences == 4 && counts[getTileType(draw)] == 1;
}

// Uncomplete
bool Hand::isIipeikou(const Tile &draw) const{
    if ( !isMenzen() ) return false;
    TileCounts counts(tile_counts);
    assert(isValidTile(draw));
    counts[getTileType(draw)]++;

    return false;

    // int sequenceCount = 0;
    // for (TileType tile : SeqBegun.list) {
    //     if (counts[tile] >= 2 && counts[tile + 1] >= 2) {
    //         sequenceCount++;
    //         counts[tile] -= 2; counts[tile + 1] -= 2;
    //     }
    // }

    // return sequenceCount == 1 && counts[getTileType(draw)] == 1;
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