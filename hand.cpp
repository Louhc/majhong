#include <algorithm>
#include <cassert>

#include "types.h"
#include "tiles.h"
#include "utility.h"

TileCounts getTileCounts(const TileList &hand) {
    TileCounts counts; counts.fill(0);
    for (Tile tile : hand)
        counts[getTileType(tile)]++;
    return counts;
}

Hand::Hand(const TileList& init_tiles, Wind round, Wind seat){
    assert(init_tiles.size() == 13);
    for ( const Tile &tile : init_tiles ) assert(isValidTile(tile));
    hand = TileList(init_tiles);
    round_wind = round;
    seat_wind = seat;
    is_menzen = true;
    tile_counts = ::getTileCounts(init_tiles);
    arrangeTiles();
    assert(isValid());
}

void Hand::arrangeTiles() {
    std::sort(hand.begin(), hand.end());
}

TileList Hand::getAllTiles() const{
    TileList all_tiles = hand;
    all_tiles.insert(all_tiles.end(), chi.begin(), chi.end());
    all_tiles.insert(all_tiles.end(), pon.begin(), pon.end());
    all_tiles.insert(all_tiles.end(), kan.begin(), kan.end());
    all_tiles.insert(all_tiles.end(), ankan.begin(), ankan.end());
    return all_tiles;
}

bool Hand::isValid() const {
    for (const auto& tile : getAllTiles()) if ( !isValidTile(tile) ) return false;
    
    if ( chi.size() % 3 != 0 || pon.size() % 3 != 0 || kan.size() % 4 != 0 || ankan.size() % 4 != 0 )
        return false;
    if ( is_menzen ) {
        if ( chi.size() > 0 || pon.size() > 0 || kan.size() > 0 )
            return false;
    } else {
        if ( chi.size() == 0 && pon.size() == 0 && kan.size() == 0 )
            return false;
        for ( int i = 0; i < chi.size(); i += 3 ) {
            if ( getSuitIndex(chi[i]) != getSuitIndex(chi[i+1]) || getSuitIndex(chi[i]) != getSuitIndex(chi[i+2]) ) return false;
            if ( getTileValue(chi[i]) + 1 != getTileValue(chi[i+1]) || getTileValue(chi[i]) + 2 != getTileValue(chi[i+2]) ) return false;
        }
        for ( int i = 0; i < pon.size(); i += 3 ) {
            if ( getTileValue(pon[i]) != getTileValue(pon[i+1]) || getTileValue(pon[i]) != getTileValue(pon[i+2]) ) return false;
        }
        for ( int i = 0; i < kan.size(); i += 4 ) {
            if ( getTileValue(kan[i]) != getTileValue(kan[i+1]) || getTileValue(kan[i]) != getTileValue(kan[i+2]) || getTileValue(kan[i]) != getTileValue(kan[i+3]) ) return false;
        }
    }
    for ( int i = 0; i < ankan.size(); i += 4 ) {
        if ( getTileValue(ankan[i]) != getTileValue(ankan[i+1]) || getTileValue(ankan[i]) != getTileValue(ankan[i+2]) || getTileValue(ankan[i]) != getTileValue(ankan[i+3]) ) return false;
    }
    if ( hand.size() + chi.size() + pon.size() + (kan.size() + ankan.size()) / 4 * 3 != 13 )
        return false;
    return true;
}

int calcKokushiMusoShanten(const TileCounts &counts) {
    int num_yao = 0; bool has_pair = 0;
    for ( const auto& i : Yao.list ) {
        if ( counts[i] >= 2 ) has_pair = true;
        if ( counts[i] > 0 ) num_yao++;
    }
    int shanten = 12 - num_yao - (has_pair ? 0 : 1);
    return shanten;
}

int calcChiitoitsuShanten(const TileCounts &counts) {
    int num_pairs = 0, num_redun = 0;
    for ( int i = 0; i < 34; ++i ) {
        if ( counts[i] >= 2 ){
            ++num_pairs;
            num_redun += counts[i] - 2;
        }
    }
    int shanten = min(6 - num_pairs, num_redun);
    return shanten;
}

// Uncompleted
int calcStandardShanten(const TileCounts &counts) {
    TileCounts a = counts;
    int num = 0; for ( Tile i = 0; i < 34; ++i ) num += a[i];
    int shanten = 8 - num;
    for ( Tile i = 0; i < 34; ++i ) {
        if ( a[i] >= 3 ){
            a[i] -= 3; shanten = min(shanten, calcStandardShanten(a)); a[i] += 3;
        }
        if ( i < 27 && i % 9 < 7 && a[i] >= 1 && a[i + 1] >= 1 && a[i + 2] >= 1 ) {
            a[i]--; a[i + 1]--; a[i + 2]--; shanten = min(shanten, calcStandardShanten(a)); a[i]++; a[i + 1]++; a[i + 2]++;
        }
    }
    return shanten;
}

int calcShanten(const TileCounts &counts) {
    int shanton = min(min(calcChiitoitsuShanten(counts), calcKokushiMusoShanten(counts)), calcStandardShanten(counts));
    return shanton;
}

int Hand::calcShanten() const {
    TileCounts counts = getTileCounts();
    int shanten = calcStandardShanten(counts);
    if ( is_menzen ) shanten = min(shanten, min(calcChiitoitsuShanten(counts), calcKokushiMusoShanten(counts)));
    return shanten;
}