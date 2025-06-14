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

HandParseResult Hand::parseWinningHand(const Tile &draw) const{
    HandParseResult res;
    TileCounts counts(tile_counts);
    assert(isValidTile(draw));
    counts[getTileType(draw)]++;

    backtrackParse(counts, {}, res, getTileType(draw), false);
    return res;
}

int calcHan( const YakuList &yaku_list, const bool &is_fuuro ){
    int han = 0;
    for ( const Yaku &yaku : yaku_list ){
        switch (yaku) {
            case Yaku::Tanyao: case Yaku::YakuhaiSelfWind: case Yaku::YakuhaiRoundWind: 
            case Yaku::YakuhaiHaku: case Yaku::YakuhaiHatsu: case Yaku::YakuhaiChun: 
            case Yaku::Pinfu: case Yaku::Iipeikou: han += 1; break;
            
            case Yaku::SanshokuDoukou: case Yaku::Sankantsu: case Yaku::Toitoi:
            case Yaku::Sanankou: case Yaku::Shousangen: case Yaku::Honroutou:
            case Yaku::Chiitoitsu: han += 2; break;
            
            case Yaku::Honchan: case Yaku::Ittsuu: case Yaku::Sanshoku: 
                han += 2 - (is_fuuro ? 1 : 0); break;
            
            case Yaku::Ryanpeikou: case Yaku::Junchan: case Yaku::Honitsu:
                han += 3; break;

            case Yaku::Chinitsu: han += 6 - (is_fuuro ? 1 : 0); break;
            
            case Yaku::Daisangen: case Yaku::Suuankou: case Yaku::Tsuuiisou:
            case Yaku::Ryuuisou: case Yaku::Chinroutou: case Yaku::KokushiMuso:
            case Yaku::Shousuushii: case Yaku::Suukantsu: case Yaku::Chuuren:
                han = 100; break;
            
            case Yaku::SuuankouTanki: case Yaku::KokushiMusoJusanmen: case Yaku::JunseiChuuren:
            case Yaku::Daisuushii: han = 200; break;
        }
    }
    return han;
}

YakuList Hand::calcYaku(const Tile &draw) const{
    YakuList yaku_list; int max_han = 0;
    assert(isValidTile(draw));

    if ( isDaisangen(draw) ) yaku_list.push_back(Yaku::Daisangen);
    if ( isSuuankou(draw) ) yaku_list.push_back(Yaku::Suuankou);
    if ( isTsuuiisou(draw) ) yaku_list.push_back(Yaku::Tsuuiisou);
    if ( isRyuuisou(draw) ) yaku_list.push_back(Yaku::Ryuuisou);
    if ( isChinroutou(draw) ) yaku_list.push_back(Yaku::Chinroutou);
    if ( isKokushiMusoJusanmen(draw) ) yaku_list.push_back(Yaku::KokushiMusoJusanmen);
    else if ( isKokushiMuso(draw) ) yaku_list.push_back(Yaku::KokushiMuso);
    if ( isDaisuushii(draw) ) yaku_list.push_back(Yaku::Daisuushii);
    else if ( isShousuushii(draw) ) yaku_list.push_back(Yaku::Shousuushii);
    if ( isSuukantsu(draw) ) yaku_list.push_back(Yaku::Suukantsu);
    if ( isJunseiChuuren(draw) ) yaku_list.push_back(Yaku::JunseiChuuren);
    else if ( isChuuren(draw) ) yaku_list.push_back(Yaku::Chuuren);
    if ( isSuuankouTanki(draw) ) yaku_list.push_back(Yaku::SuuankouTanki);
    
    if ( yaku_list.size() > 0 ) { // if is yakuman
        return yaku_list;
    }

    bool is_tanyao = isTanyao(draw),
         is_yakuhai_self_wind = isYakuhaiSelfWind(draw),
         is_yakuhai_round_wind = isYakuhaiRoundWind(draw),
         is_yakuhai_haku = isYakuhaiHaku(draw),
         is_yakuhai_hatsu = isYakuhaiHatsu(draw),
         is_yakuhai_chun = isYakuhaiChun(draw),
         is_sankantsu = isSankantsu(draw),
         is_honroutou = isHonroutou(draw),
         is_honitsu = isHonitsu(draw),
         is_chinitsu = isChinitsu(draw);

    HandParseResult parse_result = parseWinningHand(draw);
    for ( TileMeldList &melds : parse_result ) {
        YakuList meld_yaku;

        if ( is_menzen ){
            if ( melds[1].type == MeldType::ClosedSequence &&
                 melds[2].type == MeldType::ClosedSequence &&
                 melds[3].type == MeldType::ClosedSequence &&
                 melds[4].type == MeldType::ClosedSequence &&
                 melds[0].tile != Haku && melds[0].tile != Hatsu && melds[0].tile != Chun &&
                 melds[0].tile != getTileFromWind(seat_wind) && melds[0].tile != getTileFromWind(round_wind) ) {
                meld_yaku.push_back(Yaku::Pinfu);
            }

            if ( melds[1].type == MeldType::ClosedSequence &&
                 melds[2].type == MeldType::ClosedSequence &&
                 melds[3].type == MeldType::ClosedSequence &&
                 melds[4].type == MeldType::ClosedSequence &&
                 melds[1].tile == melds[2].tile && melds[3].tile == melds[4].tile ) {
                meld_yaku.push_back(Yaku::Ryanpeikou);
            } else {
                for ( int i = 1; i + 1 < melds.size(); ++i ) {
                    if ( melds[i].type == MeldType::ClosedSequence &&
                         melds[i + 1].type == MeldType::ClosedSequence &&
                         melds[i].tile == melds[i + 1].tile ) {
                        meld_yaku.push_back(Yaku::Iipeikou);
                        break;
                    }
                }
            }
        }

        for ( int i = 0; i < chi.size(); i += 3 ) melds.push_back(TileMeld(MeldType::Chi, chi[i]));
        for ( int i = 0; i < pon.size(); i += 3 ) melds.push_back(TileMeld(MeldType::Pon, pon[i]));
        for ( int i = 0; i < kan.size(); i += 4 ) melds.push_back(TileMeld(MeldType::Minkan, kan[i]));
        for ( int i = 0; i < ankan.size(); i += 4 ) melds.push_back(TileMeld(MeldType::Ankan, ankan[i]));

        if ( is_tanyao ) meld_yaku.push_back(Yaku::Tanyao);
        if ( is_yakuhai_self_wind ) meld_yaku.push_back(Yaku::YakuhaiSelfWind);
        if ( is_yakuhai_round_wind ) meld_yaku.push_back(Yaku::YakuhaiRoundWind);
        if ( is_yakuhai_haku ) meld_yaku.push_back(Yaku::YakuhaiHaku);
        if ( is_yakuhai_hatsu ) meld_yaku.push_back(Yaku::YakuhaiHatsu);
        if ( is_yakuhai_chun ) meld_yaku.push_back(Yaku::YakuhaiChun);
// Sanshoku Doukou
        std::vector<int> sanshoku_doukou_counts = {0, 0, 0, 0, 0, 0, 0, 0, 0};
        bool is_sanshoku_doukou = false;
        for ( const TileMeld &meld : melds ) {
            if ( meld.type != MeldType::Chi || 
                 meld.type != MeldType::ClosedSequence ||
                 meld.type != MeldType::Pair ) {
                sanshoku_doukou_counts[meld.tile % 9]++;
                if ( sanshoku_doukou_counts[meld.tile % 9] >= 3 ) {
                    is_sanshoku_doukou = true; break;
                }
            }
        }
        if ( is_sanshoku_doukou ) meld_yaku.push_back(Yaku::SanshokuDoukou);
// Sankantsu
        if ( is_sankantsu ) meld_yaku.push_back(Yaku::Sankantsu);
// Toitoi
        bool is_toitoi = true;
        for ( int i = 1; i < melds.size(); ++i ){
            if ( melds[i].type == MeldType::Chi || melds[i].type == MeldType::ClosedSequence ){
                is_toitoi = false;
                break;
            }
        }
        if ( is_toitoi ) meld_yaku.push_back(Yaku::Toitoi);
// Sanankou
        int ankou_count = 0;
        for ( int i = 1; i < melds.size(); ++i ) {
            if ( melds[i].type == MeldType::ClosedTriplet || melds[i].type == MeldType::Ankan ) {
                ankou_count++;
            }
        }
        if ( ankou_count >= 3 ) {
            meld_yaku.push_back(Yaku::Sanankou);
        }
// Shousangen
        if ( (is_yakuhai_chun ? 1 : 0) + (is_yakuhai_haku ? 1 : 0) +
             (is_yakuhai_hatsu ? 1 : 0) >= 2 && Sangen.contains(melds[0].tile) ) {
            meld_yaku.push_back(Yaku::Shousangen);
        }
// Honchan & Junchan
        bool is_honchan = true, is_junchan = true;
        for ( const TileMeld &meld : melds ) {
            if ( meld.type == MeldType::ClosedSequence ||
                 meld.type == MeldType::Chi ) {
                if ( !Yao.contains(meld.tile) && !Yao.contains(meld.tile + 2) ) {
                    is_honchan = is_junchan = false;
                }
            } else {
                if ( !Yao.contains(meld.tile) ) {
                    is_honchan = false;
                }
                if ( !Routou.contains(meld.tile) ) {
                    is_junchan = false;
                }
            }
        }
        if ( is_junchan ) meld_yaku.push_back(Yaku::Junchan);
        else if ( is_honchan ) meld_yaku.push_back(Yaku::Honchan);
// Ittsuu
        std::vector<int> ittsuu_counts = {0, 0, 0};
        for ( const TileMeld &meld : melds ) {
            if ( meld.type == MeldType::ClosedSequence ||
                 meld.type == MeldType::Chi && meld.tile % 3 == 0 ) {
                ittsuu_counts[meld.tile / 9] |= 1 << (meld.tile % 9 / 3);
            }
        }
        if ( ittsuu_counts[0] == 7 || ittsuu_counts[1] == 7 || ittsuu_counts[2] == 7 ) {
            meld_yaku.push_back(Yaku::Ittsuu);
        }
// Sanshoku
        std::vector<int> sanshoku_counts = {0, 0, 0, 0, 0, 0, 0, 0, 0};
        bool is_sanshoku = false;
        for ( const TileMeld &meld : melds ) {
            if ( meld.type == MeldType::Chi || 
                 meld.type == MeldType::ClosedSequence ) {
                sanshoku_doukou_counts[meld.tile % 9] |= 1 << (meld.tile / 9);
                if ( sanshoku_doukou_counts[meld.tile % 9] == 7 ) {
                    is_sanshoku_doukou = true; break;
                }
            }
        }
        if ( is_sanshoku_doukou ) meld_yaku.push_back(Yaku::SanshokuDoukou);
// Honistu & Chinitsu
        if ( is_chinitsu ) {
            meld_yaku.push_back(Yaku::Chinitsu);
        } else if ( is_honitsu ) {
            meld_yaku.push_back(Yaku::Honitsu);
        }
        
// Compare with previous max
        int han = ::calcHan(meld_yaku, !is_menzen);
        if ( han > max_han ) {
            max_han = han; yaku_list = meld_yaku;
        }
    }
    return yaku_list;
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

bool Hand::isSankantsu(const Tile &draw) const{
    if ( kan.size() + ankan.size() >= 3 * 4 ){
        return true;
    }
    return false;
}

bool Hand::isHonroutou(const Tile &draw) const{
    TileList all(getAllTiles());
    all.push_back(draw);
    for ( const Tile &tile : all ){
        if ( !Yao.contains(getTileType(tile)))
            return false;
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

bool Hand::isHonitsu(const Tile &draw) const{
    TileTypeList all(getAllTiles()); all.push_back(draw);
    int color = -1;
    for ( const Tile &tile : all ){
        if ( Honor.contains(getTileType(tile)) ) continue;
        if ( color == -1 ) color = getTileType(tile) / 9;
        else if ( color != getTileType(tile) / 9 ) return false;
    }
    return true;
}

bool Hand::isChinitsu(const Tile &draw) const{
    TileTypeList all(getAllTiles()); all.push_back(draw);
    int color = -1;
    for ( const Tile &tile : all ){
        if ( Honor.contains(getTileType(tile)) ) return false;
        if ( color == -1 ) color = getTileType(tile) / 9;
        else if ( color != getTileType(tile) / 9 ) return false;
    }
    return true;
}

bool Hand::isDaisangen(const Tile &draw) const{
    TileCounts counts(getAllTileCounts());
    counts[getTileType(draw)]++;
    for ( const TileType &tile : Sangen.list ) {
        if ( counts[tile] < 3 ) return false;
    } return true;
}

// Unable to check if it's zimo
bool Hand::isSuuankou(const Tile &draw) const{
    if ( !is_menzen ) return false;
    int ankou_counts = ankan.size() / 4, pair_counts = 0;
    TileCounts counts(tile_counts);
    counts[getTileType(draw)]++;
    for ( const TileType &tile : All.list ){
        if ( counts[tile] >= 3 ) ankou_counts++;
        else if ( counts[tile] == 2 ) pair_counts++;
    }
    return ankou_counts == 4 && pair_counts == 1;
}

bool Hand::isTsuuiisou(const Tile &draw) const{
    TileTypeList all(getAllTiles()); all.push_back(draw);
    int color = -1;
    for ( const Tile &tile : all ){
        if ( !Honor.contains(getTileType(tile)) ) return false;
    }
    return true;
}

bool Hand::isRyuuisou(const Tile &draw) const{
    TileTypeList all(getAllTiles()); all.push_back(draw);
    int color = -1;
    for ( const Tile &tile : all ){
        if ( !GreenSuited.contains(getTileType(tile)) ) return false;
    }
    return true;
}

bool Hand::isChinroutou(const Tile &draw) const{
    TileList all(getAllTiles());
    all.push_back(draw);
    for ( const Tile &tile : all ){
        if ( !Routou.contains(getTileType(tile)))
            return false;
    }
    return true;
}

bool Hand::isKokushiMuso(const Tile &draw) const{
    TileCounts counts(tile_counts);
    if ( isKokushiMusoJusanmen(draw) ) return false;
    counts[getTileType(draw)]++;

    for (TileType tile : Yao.list) {
        if (counts[tile] == 0) return false;
    }
    
    return true;
}

bool Hand::isShousuushii(const Tile &draw) const{
    TileCounts counts(getAllTileCounts());
    counts[getTileType(draw)]++;
    int pair_counts = 0, triplet_counts = 0;
    for ( const TileType &tile : Kaze.list ) {
        if ( counts[tile] == 2 ) pair_counts++;
        if ( counts[tile] >= 3 ) triplet_counts++;
    } return pair_counts <= 1 && triplet_counts >= 3;
}

bool Hand::isSuukantsu(const Tile &draw) const{
    return kan.size() + ankan.size() >= 4 && getTileType(draw) == getTileType(hand[0]);
}

bool Hand::isChuuren(const Tile &draw) const{
    // if ( !isChinitsu(draw) ) return false;
    return false;
}

bool Hand::isSuuankouTanki(const Tile &draw) const{
    return false;
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

bool Hand::isJunseiChuuren(const Tile &draw) const{
    return false;
}

bool Hand::isDaisuushii(const Tile &draw) const{
    TileCounts counts(getAllTileCounts());
    counts[getTileType(draw)]++;
    for ( const TileType &tile : Kaze.list ) {
        if ( counts[tile] < 3 ) return false;
    }
    return true;
}
