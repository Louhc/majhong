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

TileIndexList::iterator findByTileIndex( TileIndexList &hand, const TileIndex &tile_index ){
    for ( int i = 0; i < hand.size(); ++i ) {
        if ( hand[i] == tile_index ) return hand.begin() + i;
    }
    return hand.end();
}

TileIndexList::iterator findByTile( TileIndexList &hand, const Tile &tile, bool is_red_dragon = false ){
    for ( int i = 0; i < hand.size(); ++i ) {
        if ( hand[i] / 4 == tile && (!is_red_dragon || !Five.contains(tile) || hand[i] % 4 == 0) ) return hand.begin() + i;
    }
    return hand.end();
}

bool Hand::callChi(const TileIndex &call, const TileIndex &discard, int opt){
    Tile t0 = call / 4, t1, t2;
    if ( opt % 3 == 0 ) t1 = getPrevTile(call), t2 = getPrevTile(t1);
    else if ( opt % 3 == 1 ) t1 = getPrevTile(call), t2 = getNextTile(call);
    else if ( opt % 3 == 2 ) t1 == getNextTile(call), t2 = getNextTile(t1);
    auto it1 = findByTile(hand, t1, opt > 2 && Five.contains(t1)); assert(it1 != hand.end());
    open.push_back(*it1); hand.erase(it1); tile_counts[t1]--;
    auto it2 = findByTile(hand, t2, opt > 2 && Five.contains(t2)); assert(it2 != hand.end());
    open.push_back(*it2); hand.erase(it2); tile_counts[t2]--;
    open.push_back(call);

    auto it = findByTileIndex(hand, discard); assert(it != hand.end());
    hand.erase(it); tile_counts[discard / 4]--;

    if ( opt == 2 ) open_melds.push_back(TileMeld(MeldType::Chi, t0));
    else open_melds.push_back(TileMeld(MeldType::Chi, t1));
    is_menzen = false;

    return true;
}

bool Hand::callPon(const TileIndex &call, const TileIndex &discard, int opt = 0){
    Tile tile = call / 4;
    auto it1 = findByTile(hand, tile, opt == 1); assert(it1 != hand.end());
    open.push_back(*it1); hand.erase(it1); tile_counts[tile]--;
    auto it2 = findByTile(hand, tile); assert(it2 != hand.end());
    open.push_back(*it2); hand.erase(it2); tile_counts[tile]--;
    open.push_back(call);
    
    auto it = findByTileIndex(hand, discard); assert(it != hand.end());
    hand.erase(it); tile_counts[discard / 4]--;

    open_melds.push_back(TileMeld(MeldType::Pon, tile));
    is_menzen = false;

    return true;
}

bool Hand::callKan(const TileIndex &call){
    Tile tile = call / 4;
    for ( int i = 0; i < 3; ++i ){
        auto it = findByTile(hand, tile); assert(it != hand.end());
        open.push_back(*it); hand.erase(it); tile_counts[tile]--;
    }
    open.push_back(call);

    open_melds.push_back(TileMeld(MeldType::Minkan, tile));
    is_menzen = false;

    return true;
}

bool Hand::performAnkan(const TileIndex &tile_index){
    Tile tile = tile_index / 4;
    for ( int i = 0; i < 3; ++i ){
        auto it = findByTile(hand, tile); assert(it != hand.end());
        open.push_back(*it); hand.erase(it); tile_counts[tile]--;
    }
    open.push_back(tile_index);

    open_melds.push_back(TileMeld(MeldType::Ankan, tile));

    return true;
}

bool Hand::performChakan(const TileIndex &tile_index){
    Tile tile = tile_index / 4; bool is_found = false;
    for ( int i = 0; i < open_melds.size(); ++i ){
        if ( open_melds[i].type == MeldType::Pon && open_melds[i].tile == tile ){
            open_melds[i].type = MeldType::Chakan; is_found = true;
            break;
        }
    }
    assert(is_found);
    open.push_back(tile_index);

    return true;
}

bool Hand::drawAndDiscard(const Tile &draw, const Tile &discard){
    hand.push_back(draw);
    tile_counts[draw / 4]++;

    auto it = findByTile(hand, discard); assert(it != hand.end());
    hand.erase(it); tile_counts[discard / 4]--;
    
    return true;
}

/////////////////////////////////////////////////////////////////////////
