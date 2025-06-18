#include <algorithm>
#include <cassert>

#include "types.h"
#include "tiles.h"
#include "utils.h"
#include "constants.h"

TileCounts getTileCounts(const TileIndexList &hand) {
    TileCounts counts; counts.fill(0);
    for (TileIndex tile_index : hand)
        counts[tile_index / 4]++;
    return counts;
}

Hand::Hand(const TileIndexList& init_tiles, Wind round, Wind seat){
    assert(init_tiles.size() == 13);
    // for ( const TileIndex &tile_index : init_tiles ) 
    //     assert(tile_index >= 0 && tile_index < 136);
    hand = TileList(init_tiles);
    open_melds.clear();
    round_wind = round;
    seat_wind = seat;
    tile_counts = ::getTileCounts(init_tiles);
    is_menzen = 1; is_richii = 0;
}

void Hand::arrangeTiles() {
    std::sort(hand.begin(), hand.end());
}

TileList Hand::getAllTiles() const{
    TileList all_tiles = hand;
    all_tiles.insert(all_tiles.end(), open.begin(), open.end());
    return all_tiles;
}

TileCounts Hand::getAllTileCounts() const{
    TileCounts all_counts = tile_counts;
    for ( const Tile &tile : open ) all_counts[tile / 4]++;
    return all_counts;
}

/////////////////////////////////////////////////////////////////////////
// Chi, Pon, Kan

Tile getPrevTile( const Tile &tile ) {
    if ( tile == invalid_tile || Honor.contains(tile) || tile == _1m || tile == _1p || tile == _1s )
        return invalid_tile;
    return tile - 1;
}

Tile getNextTile( const Tile &tile ) {
    if ( tile == invalid_tile || Honor.contains(tile) || tile == _9m || tile == _9p || tile == _9s )
        return invalid_tile;
    return tile + 1;
}

bool Hand::canChi(const TileIndex &call) const{
    Tile tile = call / 4;
    if ( tile_counts[getPrevTile(tile)] > 0 ) {
        if ( tile_counts[getPrevTile(getPrevTile(tile))] > 0 )
            return true;
        if ( tile_counts[getNextTile(tile)] > 0 )
            return true;
    } else {
        if ( tile_counts[getNextTile(getNextTile(tile))] > 0 )
            return true;
    }
    return false;
}

bool Hand::canPon(const TileIndex &call) const{
    return tile_counts[call / 4] >= 2;
}

bool Hand::canKan(const TileIndex &call) const{
    return tile_counts[call / 4] == 3;
}

bool Hand::canAnkan(const TileIndex &tile_index) const{
    return tile_counts[tile_index / 4] == 3;
}

bool Hand::callChi(const TileIndex &call, const TileIndex &discard, int opt){
    return false;
}

bool Hand::callPon(const TileIndex &call, const TileIndex &discard, int opt){
    if ( !canPon(call) || discard / 4 == call / 4 ) return false;

    bool is_discarded = false;
    for ( int i = 0; i < hand.size(); ++i ) {
        if ( hand[i] == discard ){
            hand.erase(hand.begin() + i);
            is_discarded = true;
            break;
        }
    }
    if ( !is_discarded ) return false;
    tile_counts[discard / 4]--;

    Tile tile = call / 4;
    int tot = 0; 
    for ( int i = 0; i < hand.size(); ++i ){
        if ( hand[i] / 4 == tile ){
            ++tot;
            open.push_back(hand[i]);
            hand.erase(hand.begin() + i);
            --i;
            if ( tot == 2 ) break;
        }
    }
    open_melds.push_back(TileMeld(MeldType::Pon, tile));
    tile_counts[tile] -= 2;
    is_menzen = false;
    return true;
}

bool Hand::callKan(const TileIndex &call){
    if ( !canKan(call) ) return false;
    Tile tile = call / 4;
    int tot = 0;
    for ( int i = 0; i < hand.size(); ++i ){
        if ( hand[i] / 4 == tile ){
            ++tot;
            open.push_back(hand[i]);
            hand.erase(hand.begin() + i);
            --i;
            if ( tot == 3 ) break;
        }        
    }
    open_melds.push_back(TileMeld(MeldType::Minkan, tile));
    tile_counts[tile] -= 3;
    is_menzen = false;
    return true;
}

bool Hand::performAnkan(const TileIndex &tile_index, const TileIndex &draw){
    if ( tile_counts[tile_index / 4] != 4 ) return false;
    Tile tile = tile_index / 4;
    int tot = 0;
    for ( int i = 0; i < hand.size(); ++i ){
        if ( hand[i] / 4 == tile ){
            ++tot;
            open.push_back(hand[i]);
            hand.erase(hand.begin() + i);
            --i;
            if ( tot == 4 ) break;
        }        
    }
    open_melds.push_back(TileMeld(MeldType::Ankan, tile));
    tile_counts[tile] -= 4;

    hand.push_back(draw);
    tile_counts[draw / 4]++;
    return true;
}

bool Hand::drawAndDiscard(const Tile &draw, const Tile &discard){
    hand.push_back(draw);
    tile_counts[draw / 4]++;

    bool is_discarded = false;
    for ( int i = 0; i < hand.size(); ++i ) {
        if ( hand[i] == discard ){
            hand.erase(hand.begin() + i);
            is_discarded = true;
            break;
        }
    }
    if ( !is_discarded ) return false;
    tile_counts[discard / 4]--;
    return true;
}

/////////////////////////////////////////////////////////////////////////

bool Hand::isValid() const {
    return true;
}