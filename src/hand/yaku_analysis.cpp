#include <cassert>
#include <algorithm>

#include "types.h"
#include "constants.h"
#include "tiles.h"
#include "scoring.h"

Tile getTileFromWind(const Wind &wind) {
    switch (wind) {
        case Wind::East: return EastWind;
        case Wind::South: return SouthWind;
        case Wind::West: return WestWind;
        case Wind::North: return NorthWind;
        default: return invalid_tile; // Invalid wind
    }
}

bool backtrackParse( TileCounts current_counts, TileMeldList current_melds,
        HandParseResult& all_results, bool is_pair_found, int enumerator = 0 ){
    int sum = 0;
    for ( const Tile &tile : All.list )
        sum += current_counts[tile];
    if ( sum == 0 ){
        all_results.push_back(current_melds);
        return true;
    }
    bool flag = false;
    for ( const Tile &tile : All.list ){
        if ( current_counts[tile] == 0 ) continue;

        // Check for sequences
        if ( tile * 3 >= enumerator && SeqBegun.contains(tile) && current_counts[tile + 1] > 0 && current_counts[tile + 2] > 0 ){
            current_counts[tile]--; current_counts[tile + 1]--; current_counts[tile + 2]--;
            current_melds.push_back(TileMeld(MeldType::ClosedSequence, tile));
            if ( backtrackParse(current_counts, current_melds, all_results, is_pair_found, tile * 3) )
                flag =  true;
            current_melds.pop_back();
            current_counts[tile]++; current_counts[tile + 1]++; current_counts[tile + 2]++;
        }

        // Check for pairs
        if ( tile * 3 + 1 >= enumerator && !is_pair_found && current_counts[tile] >= 2 ){
            current_counts[tile] -= 2;
            current_melds.push_back(TileMeld(MeldType::Pair, tile));
            std::swap(current_melds[0], current_melds.back());
            if ( backtrackParse(current_counts, current_melds, all_results, true, tile * 3 + 1) )
                flag = true;
            std::swap(current_melds[0], current_melds.back());
            current_melds.pop_back();
            current_counts[tile] += 2;
        }

        // Check for triplets
        if ( tile * 3 + 2 >= enumerator && current_counts[tile] >= 3 ){
            current_counts[tile] -= 3;
            current_melds.push_back(TileMeld(MeldType::ClosedTriplet, tile));
            if ( backtrackParse(current_counts, current_melds, all_results, is_pair_found, tile * 3 + 2) )
                flag = true;
            current_melds.pop_back();
            current_counts[tile] += 3;
        }
    }
    return flag;
}

HandParseResult Hand::parseWinningHand(const TileIndex &draw) const{
    HandParseResult res;
    TileCounts counts(tile_counts); counts[draw / 4]++;

    backtrackParse(counts, {}, res, false);
    return res;
}

int calcHan( const YakuList &yaku_list, const bool &is_fuuro ){
    int han = 0;
    for ( const Yaku &yaku : yaku_list ){
        switch (yaku) {
            // 1翻役
            case Yaku::Richii: case Yaku::Tsumo: case Yaku::Ippatsu:
            case Yaku::Tanyao: case Yaku::YakuhaiSelfWind: case Yaku::YakuhaiRoundWind:
            case Yaku::YakuhaiHaku: case Yaku::YakuhaiHatsu: case Yaku::YakuhaiChun:
            case Yaku::Pinfu: case Yaku::Iipeikou:
            case Yaku::Chankan: case Yaku::Rinshan: case Yaku::Haitei: case Yaku::Houtei:
                han += 1; break;

            // 2翻役
            case Yaku::DoubleRichii: han += 2; break;
            case Yaku::SanshokuDoukou: case Yaku::Sankantsu: case Yaku::Toitoi:
            case Yaku::Sanankou: case Yaku::Shousangen: case Yaku::Honroutou:
            case Yaku::Chiitoitsu: han += 2; break;

            // 2翻役 (副露降1翻)
            case Yaku::Honchan: case Yaku::Ittsuu: case Yaku::Sanshoku:
                han += 2 - (is_fuuro ? 1 : 0); break;

            // 3翻役 (副露降1翻)
            case Yaku::Ryanpeikou: han += 3; break;
            case Yaku::Junchan: case Yaku::Honitsu:
                han += 3 - (is_fuuro ? 1 : 0); break;

            // 6翻役 (副露降1翻)
            case Yaku::Chinitsu: han += 6 - (is_fuuro ? 1 : 0); break;

            // 役满
            case Yaku::Daisangen: case Yaku::Suuankou: case Yaku::Tsuuiisou:
            case Yaku::Ryuuisou: case Yaku::Chinroutou: case Yaku::KokushiMuso:
            case Yaku::Shousuushii: case Yaku::Suukantsu: case Yaku::Chuuren:
                han = 100; break;

            // 双倍役满
            case Yaku::SuuankouTanki: case Yaku::KokushiMusoJusanmen: case Yaku::JunseiChuuren:
            case Yaku::Daisuushii: han = 200; break;
        }
    }
    return han;
}

YakuList Hand::calcYaku(const TileIndex &draw, const bool &is_tsumo) const{
    YakuList yaku_list; int max_han = 0;

    if ( isDaisangen(draw) ) yaku_list.push_back(Yaku::Daisangen);
    if ( isSuuankou(draw, is_tsumo) ) yaku_list.push_back(Yaku::Suuankou);
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

    if ( isChiitoitsu(draw) ){ // if is chiitoitsu
        yaku_list.push_back(Yaku::Chiitoitsu);
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
            if ( is_tsumo ){
                meld_yaku.push_back(Yaku::Tsumo);
            }
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

        for ( int i = 0; i < open_melds.size(); ++i ) melds.push_back(open_melds[i]);

        if ( is_tanyao ) meld_yaku.push_back(Yaku::Tanyao);
        if ( is_yakuhai_self_wind ) meld_yaku.push_back(Yaku::YakuhaiSelfWind);
        if ( is_yakuhai_round_wind ) meld_yaku.push_back(Yaku::YakuhaiRoundWind);
        if ( is_yakuhai_haku ) meld_yaku.push_back(Yaku::YakuhaiHaku);
        if ( is_yakuhai_hatsu ) meld_yaku.push_back(Yaku::YakuhaiHatsu);
        if ( is_yakuhai_chun ) meld_yaku.push_back(Yaku::YakuhaiChun);
// Sanshoku Doukou (三色同刻)
        std::vector<int> sanshoku_doukou_counts = {0, 0, 0, 0, 0, 0, 0, 0, 0};
        bool is_sanshoku_doukou = false;
        for ( const TileMeld &meld : melds ) {
            // 只检查刻子/杠，不检查顺子和雀头
            if ( meld.type != MeldType::Chi &&
                 meld.type != MeldType::ClosedSequence &&
                 meld.type != MeldType::Pair &&
                 meld.tile < 27 ) {  // 只有数牌才能三色同刻
                sanshoku_doukou_counts[meld.tile % 9] |= 1 << (meld.tile / 9);
                if ( sanshoku_doukou_counts[meld.tile % 9] == 7 ) {
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
// Honroutou (混老头)
        if ( is_honroutou ) meld_yaku.push_back(Yaku::Honroutou);
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
                 (meld.type == MeldType::Chi && meld.tile % 3 == 0) ) {
                ittsuu_counts[meld.tile / 9] |= 1 << (meld.tile % 9 / 3);
            }
        }
        if ( ittsuu_counts[0] == 7 || ittsuu_counts[1] == 7 || ittsuu_counts[2] == 7 ) {
            meld_yaku.push_back(Yaku::Ittsuu);
        }
// Sanshoku (三色同顺)
        std::vector<int> sanshoku_counts = {0, 0, 0, 0, 0, 0, 0};
        bool is_sanshoku = false;
        for ( const TileMeld &meld : melds ) {
            if ( (meld.type == MeldType::Chi ||
                  meld.type == MeldType::ClosedSequence) &&
                 meld.tile < 27 ) {
                sanshoku_counts[meld.tile % 9] |= 1 << (meld.tile / 9);
                if ( sanshoku_counts[meld.tile % 9] == 7 ) {
                    is_sanshoku = true; break;
                }
            }
        }
        if ( is_sanshoku ) meld_yaku.push_back(Yaku::Sanshoku);
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

// 新增：支持完整状态标志的 calcYaku
YakuList Hand::calcYaku(const TileIndex &draw, const AgariFlags &flags) const {
    YakuList yaku_list;
    int max_han = 0;

    // 检查役满
    if ( isDaisangen(draw) ) yaku_list.push_back(Yaku::Daisangen);
    if ( isSuuankou(draw, flags.is_tsumo) ) yaku_list.push_back(Yaku::Suuankou);
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

    // 特殊役满
    if ( flags.is_tenhou ) {
        yaku_list.clear();
        yaku_list.push_back(Yaku::Daisangen);  // 用大三元代替天和 (需要添加天和枚举)
        return yaku_list;
    }
    if ( flags.is_chihou ) {
        yaku_list.clear();
        yaku_list.push_back(Yaku::Daisangen);  // 用大三元代替地和 (需要添加地和枚举)
        return yaku_list;
    }

    if ( !yaku_list.empty() ) { // 如果有役满
        return yaku_list;
    }

    if ( isChiitoitsu(draw) ) {
        yaku_list.push_back(Yaku::Chiitoitsu);
        // 七对子也可以叠加其他役
        if ( isTanyao(draw) ) yaku_list.push_back(Yaku::Tanyao);
        if ( isHonroutou(draw) ) yaku_list.push_back(Yaku::Honroutou);
        if ( isHonitsu(draw) ) yaku_list.push_back(Yaku::Honitsu);
        if ( isChinitsu(draw) ) yaku_list.push_back(Yaku::Chinitsu);
        // 添加状态役
        if ( flags.is_riichi ) yaku_list.push_back(Yaku::Richii);
        if ( flags.is_double_riichi ) yaku_list.push_back(Yaku::DoubleRichii);
        if ( flags.is_ippatsu ) yaku_list.push_back(Yaku::Ippatsu);
        if ( flags.is_tsumo && is_menzen ) yaku_list.push_back(Yaku::Tsumo);
        if ( flags.is_haitei ) yaku_list.push_back(Yaku::Haitei);
        if ( flags.is_houtei ) yaku_list.push_back(Yaku::Houtei);
        return yaku_list;
    }

    // 预计算不依赖面子解析的役
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

        // 状态役 (不依赖面子)
        if ( flags.is_riichi ) meld_yaku.push_back(Yaku::Richii);
        if ( flags.is_double_riichi ) meld_yaku.push_back(Yaku::DoubleRichii);
        if ( flags.is_ippatsu ) meld_yaku.push_back(Yaku::Ippatsu);
        if ( flags.is_rinshan ) meld_yaku.push_back(Yaku::Rinshan);
        if ( flags.is_chankan ) meld_yaku.push_back(Yaku::Chankan);
        if ( flags.is_haitei ) meld_yaku.push_back(Yaku::Haitei);
        if ( flags.is_houtei ) meld_yaku.push_back(Yaku::Houtei);

        if ( is_menzen ) {
            if ( flags.is_tsumo ) {
                meld_yaku.push_back(Yaku::Tsumo);
            }
            // 平和判定
            if ( melds.size() >= 5 &&
                 melds[1].type == MeldType::ClosedSequence &&
                 melds[2].type == MeldType::ClosedSequence &&
                 melds[3].type == MeldType::ClosedSequence &&
                 melds[4].type == MeldType::ClosedSequence &&
                 melds[0].tile != Haku && melds[0].tile != Hatsu && melds[0].tile != Chun &&
                 melds[0].tile != getTileFromWind(seat_wind) && melds[0].tile != getTileFromWind(round_wind) ) {
                meld_yaku.push_back(Yaku::Pinfu);
            }

            // 二杯口 / 一杯口
            if ( melds.size() >= 5 &&
                 melds[1].type == MeldType::ClosedSequence &&
                 melds[2].type == MeldType::ClosedSequence &&
                 melds[3].type == MeldType::ClosedSequence &&
                 melds[4].type == MeldType::ClosedSequence &&
                 melds[1].tile == melds[2].tile && melds[3].tile == melds[4].tile ) {
                meld_yaku.push_back(Yaku::Ryanpeikou);
            } else {
                for ( size_t i = 1; i + 1 < melds.size(); ++i ) {
                    if ( melds[i].type == MeldType::ClosedSequence &&
                         melds[i + 1].type == MeldType::ClosedSequence &&
                         melds[i].tile == melds[i + 1].tile ) {
                        meld_yaku.push_back(Yaku::Iipeikou);
                        break;
                    }
                }
            }
        }

        // 加入副露
        for ( size_t i = 0; i < open_melds.size(); ++i ) melds.push_back(open_melds[i]);

        if ( is_tanyao ) meld_yaku.push_back(Yaku::Tanyao);
        if ( is_yakuhai_self_wind ) meld_yaku.push_back(Yaku::YakuhaiSelfWind);
        if ( is_yakuhai_round_wind ) meld_yaku.push_back(Yaku::YakuhaiRoundWind);
        if ( is_yakuhai_haku ) meld_yaku.push_back(Yaku::YakuhaiHaku);
        if ( is_yakuhai_hatsu ) meld_yaku.push_back(Yaku::YakuhaiHatsu);
        if ( is_yakuhai_chun ) meld_yaku.push_back(Yaku::YakuhaiChun);

        // 三色同刻
        std::vector<int> sanshoku_doukou_counts = {0, 0, 0, 0, 0, 0, 0, 0, 0};
        bool is_sanshoku_doukou = false;
        for ( const TileMeld &meld : melds ) {
            if ( meld.type != MeldType::Chi &&
                 meld.type != MeldType::ClosedSequence &&
                 meld.type != MeldType::Pair &&
                 meld.tile < 27 ) {
                sanshoku_doukou_counts[meld.tile % 9] |= 1 << (meld.tile / 9);
                if ( sanshoku_doukou_counts[meld.tile % 9] == 7 ) {
                    is_sanshoku_doukou = true; break;
                }
            }
        }
        if ( is_sanshoku_doukou ) meld_yaku.push_back(Yaku::SanshokuDoukou);

        if ( is_sankantsu ) meld_yaku.push_back(Yaku::Sankantsu);

        // 对对和
        bool is_toitoi = true;
        for ( size_t i = 1; i < melds.size(); ++i ) {
            if ( melds[i].type == MeldType::Chi || melds[i].type == MeldType::ClosedSequence ) {
                is_toitoi = false;
                break;
            }
        }
        if ( is_toitoi ) meld_yaku.push_back(Yaku::Toitoi);

        if ( is_honroutou ) meld_yaku.push_back(Yaku::Honroutou);

        // 三暗刻
        int ankou_count = 0;
        for ( size_t i = 1; i < melds.size(); ++i ) {
            if ( melds[i].type == MeldType::ClosedTriplet || melds[i].type == MeldType::Ankan ) {
                ankou_count++;
            }
        }
        if ( ankou_count >= 3 ) {
            meld_yaku.push_back(Yaku::Sanankou);
        }

        // 小三元
        if ( (is_yakuhai_chun ? 1 : 0) + (is_yakuhai_haku ? 1 : 0) +
             (is_yakuhai_hatsu ? 1 : 0) >= 2 && Sangen.contains(melds[0].tile) ) {
            meld_yaku.push_back(Yaku::Shousangen);
        }

        // 混全带/纯全带
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

        // 一气通贯
        std::vector<int> ittsuu_counts = {0, 0, 0};
        for ( const TileMeld &meld : melds ) {
            if ( meld.type == MeldType::ClosedSequence ||
                 (meld.type == MeldType::Chi && meld.tile % 3 == 0) ) {
                ittsuu_counts[meld.tile / 9] |= 1 << (meld.tile % 9 / 3);
            }
        }
        if ( ittsuu_counts[0] == 7 || ittsuu_counts[1] == 7 || ittsuu_counts[2] == 7 ) {
            meld_yaku.push_back(Yaku::Ittsuu);
        }

        // 三色同顺
        std::vector<int> sanshoku_counts = {0, 0, 0, 0, 0, 0, 0};
        bool is_sanshoku = false;
        for ( const TileMeld &meld : melds ) {
            if ( (meld.type == MeldType::Chi ||
                  meld.type == MeldType::ClosedSequence) &&
                 meld.tile < 27 ) {
                sanshoku_counts[meld.tile % 9] |= 1 << (meld.tile / 9);
                if ( sanshoku_counts[meld.tile % 9] == 7 ) {
                    is_sanshoku = true; break;
                }
            }
        }
        if ( is_sanshoku ) meld_yaku.push_back(Yaku::Sanshoku);

        // 混一色 / 清一色
        if ( is_chinitsu ) {
            meld_yaku.push_back(Yaku::Chinitsu);
        } else if ( is_honitsu ) {
            meld_yaku.push_back(Yaku::Honitsu);
        }

        // 选择翻数最高的
        int han = ::calcHan(meld_yaku, !is_menzen);
        if ( han > max_han ) {
            max_han = han;
            yaku_list = meld_yaku;
        }
    }
    return yaku_list;
}

bool Hand::isTanyao(const TileIndex &draw) const{
    if ( Yao.containsIdx(draw) ) return false;
    for ( const Tile &tile : Yao.list )
        if ( tile_counts[tile] > 0) return false;
    return true;
}

bool Hand::isYakuhaiSelfWind(const TileIndex &draw) const{
    TileCounts counts(getAllTileCounts());
    counts[draw / 4]++;
    return counts[getTileFromWind(seat_wind)] >= 3;
}

bool Hand::isYakuhaiRoundWind(const TileIndex &draw) const{
    TileCounts counts(getAllTileCounts());
    counts[draw / 4]++;
    return counts[getTileFromWind(round_wind)] >= 3;
}

bool Hand::isYakuhaiHaku(const TileIndex &draw) const{
    TileCounts counts(getAllTileCounts());
    counts[draw / 4]++;
    return counts[Haku] >= 3;
}

bool Hand::isYakuhaiHatsu(const TileIndex &draw) const{
    TileCounts counts(getAllTileCounts());
    counts[draw / 4]++;
    return counts[Hatsu] >= 3;
}

bool Hand::isYakuhaiChun(const TileIndex &draw) const{
    TileCounts counts(getAllTileCounts());
    counts[draw / 4]++;
    return counts[Chun] >= 3;
}

bool Hand::isSankantsu(const TileIndex &draw) const{
    int num_kan = 0;
    for ( const auto &meld : open_melds )
        if ( meld.type == MeldType::Ankan || meld.type == MeldType::Minkan || meld.type == MeldType::Chakan )
            ++num_kan;
    return num_kan >= 3;
}

bool Hand::isHonroutou(const TileIndex &draw) const{
    TileIndexList all(getAllTiles());
    all.push_back(draw);
    for ( const TileIndex &tile_index : all ){
        if ( !Yao.containsIdx(tile_index) )
            return false;
    }
    return true;
}

bool Hand::isChiitoitsu(const TileIndex &draw) const{
    if ( !is_menzen ) return false;

    TileCounts counts(tile_counts);
    counts[draw / 4]++;

    int num_pair = 0;
    for ( const Tile &tile : All.list ){
        if ( counts[tile] == 2 ) num_pair++;
    }
    return num_pair == 7;
}

bool Hand::isHonitsu(const TileIndex &draw) const{
    TileIndexList all(getAllTiles()); all.push_back(draw);
    int color = -1;
    for ( const TileIndex &tile_index : all ){
        if ( Honor.containsIdx(tile_index) ) continue;
        if ( color == -1 ) color = tile_index / 4 / 9;
        else if ( color != tile_index / 4 / 9 ) return false;
    }
    return true;
}

bool Hand::isChinitsu(const TileIndex &draw) const{
    TileIndexList all(getAllTiles()); all.push_back(draw);
    int color = -1;
    for ( const TileIndex &tile_index : all ){
        if ( Honor.containsIdx(tile_index) ) return false;
        if ( color == -1 ) color = tile_index / 4 / 9;
        else if ( color != tile_index / 4 / 9 ) return false;
    }
    return true;
}

bool Hand::isDaisangen(const TileIndex &draw) const{
    TileCounts counts(getAllTileCounts()); counts[draw / 4]++;
    for ( const Tile &tile : Sangen.list ) {
        if ( counts[tile] < 3 ) return false;
    } return true;
}

bool Hand::isSuuankou(const TileIndex &draw, const bool &is_tsumo) const{
    if ( !is_menzen || !is_tsumo ) return false;
    int ankou_num = 0, pair_num = 0;
    for ( int i = 0; i < open_melds.size(); ++i )
        if ( open_melds[i].type == MeldType::Ankan )
            ++ankou_num;

    TileCounts counts(tile_counts);
    counts[draw / 4]++;
    for ( const Tile &tile : All.list ){
        if ( counts[tile] >= 3 ) ankou_num++;
        else if ( counts[tile] == 2 ) pair_num++;
    }

    return ankou_num == 4 && pair_num == 1;
}

bool Hand::isTsuuiisou(const TileIndex &draw) const{
    TileIndexList all(getAllTiles()); all.push_back(draw);
    for ( const TileIndex &tile_index : all ){
        if ( !Honor.contains(tile_index / 4) ) return false;
    }
    return true;
}

bool Hand::isRyuuisou(const TileIndex &draw) const{
    TileIndexList all(getAllTiles()); all.push_back(draw);
    for ( const TileIndex &tile_index : all ){
        if ( !GreenSuited.contains(tile_index / 4) ) return false;
    }
    return true;
}

bool Hand::isChinroutou(const TileIndex &draw) const{
    TileIndexList all(getAllTiles()); all.push_back(draw);
    for ( const TileIndex &tile_index : all )
        if ( !Routou.contains(tile_index / 4) )
            return false;
    return true;
}

bool Hand::isKokushiMuso(const TileIndex &draw) const{
    TileCounts counts(tile_counts); counts[draw / 4]++;

    for (const Tile &tile : Yao.list) {
        if (counts[tile] == 0) return false;
    }
    
    return true;
}

bool Hand::isShousuushii(const TileIndex &draw) const{
    TileCounts counts(getAllTileCounts()); counts[draw / 4]++;
    int pair_num = 0, triplet_num = 0;
    for ( const Tile &tile : Kaze.list ) {
        if ( counts[tile] == 2 ) pair_num++;
        if ( counts[tile] >= 3 ) triplet_num++;
    } 
    
    return pair_num == 1 && triplet_num == 3;
}

bool Hand::isSuukantsu(const Tile &draw) const{
    if ( draw / 4 != hand[0] / 4 ) return false;
    int num_kan = 0;
    for ( int i = 0; i < open_melds.size(); ++i )
        if ( open_melds[i].type == MeldType::Ankan || open_melds[i].type == MeldType::Minkan || open_melds[i].type == MeldType::Chakan )
            ++num_kan;
    return num_kan == 4;
}

bool Hand::isChuuren(const TileIndex &draw) const{
    if ( !isChinitsu(draw) || !is_menzen ) return false;
    int start = draw / 4 / 9 * 9;
    TileCounts counts(getAllTileCounts()); counts[draw / 4]++;
    if ( counts[start] < 3 || counts[start + 8] < 3 )
        return false;
    for ( int i = 1; i < 8; ++i )
        if ( counts[start + i] < 1 )
            return false;
    return true;
}

bool Hand::isSuuankouTanki(const TileIndex &draw) const{
    if ( !is_menzen ) return false;
    int ankou_num = 0, pair_num = 0;
    for ( int i = 0; i < open_melds.size(); ++i )
        if ( open_melds[i].type == MeldType::Ankan )
            ++ankou_num;

    TileCounts counts(tile_counts);
    for ( const Tile &tile : All.list ){
        if ( counts[tile] >= 3 ) ankou_num++;
        else if ( counts[tile] == 1 && tile == draw / 4 ) pair_num++;
    }

    return ankou_num == 4 && pair_num == 1;
}

bool Hand::isKokushiMusoJusanmen(const TileIndex &draw) const{
    TileCounts counts(tile_counts);
    if ( !Yao.containsIdx(draw) ) return false;
    for (const Tile &tile : Yao.list) {
        if (counts[tile] == 0) return false;
    }

    return true;
}

bool Hand::isJunseiChuuren(const TileIndex &draw) const{
    if ( !isChinitsu(draw) || !is_menzen ) return false;
    int start = draw / 4 / 9 * 9;
    TileCounts counts(getAllTileCounts());
    if ( counts[start] < 3 || counts[start + 8] < 3 )
        return false;
    for ( int i = 1; i < 8; ++i )
        if ( counts[start + i] < 1 )
            return false;
    return true;
}

bool Hand::isDaisuushii(const TileIndex &draw) const{
    TileCounts counts(getAllTileCounts()); counts[draw / 4]++;
    for ( const Tile &tile : Kaze.list ) {
        if ( counts[tile] < 3 ) return false;
    }
    return true;
}

bool Hand::isWinningHand(const TileIndex &draw) const{
    // Check for Kokushi Muso (special hand that doesn't parse normally)
    if ( isKokushiMuso(draw) ) return true;
    // Check for Chiitoitsu (7 pairs)
    if ( isChiitoitsu(draw) ) return true;
    // Check for standard winning hand (4 melds + 1 pair)
    HandParseResult res = parseWinningHand(draw);
    return !res.empty();
}

int Hand::calcHan() const{
    // This method needs a draw tile to calculate yaku
    // For now, return 0 as placeholder - actual calculation done via calcYaku
    return 0;
}

// Helper function for shanten calculation
static int calcShantenNormal(TileCounts counts, int melds, int tatsu, bool has_pair, int idx) {
    if ( idx == 34 ) {
        int need_melds = 4 - melds;
        int need_tatsu = std::min(tatsu, need_melds);
        // Formula: how many tiles needed to complete
        // If has_pair: need to complete melds only
        // If no pair: need to complete melds AND get a pair (but if 4 melds, just need pair completion = 0)
        int shanten;
        if (has_pair) {
            shanten = need_melds * 2 - need_tatsu;
        } else {
            // Without pair, need one more tile for the pair
            // But if we have 4 melds and no pair, we're tenpai (waiting for the pair tile)
            if (need_melds == 0) {
                shanten = 0; // Tenpai - waiting for pair
            } else {
                shanten = need_melds * 2 - need_tatsu;
            }
        }
        return shanten;
    }

    int res = 8; // Maximum shanten

    // Try triplet
    if ( counts[idx] >= 3 ) {
        counts[idx] -= 3;
        res = std::min(res, calcShantenNormal(counts, melds + 1, tatsu, has_pair, idx));
        counts[idx] += 3;
    }

    // Try sequence (only for numbered tiles 0-26)
    if ( idx < 27 && idx % 9 <= 6 && counts[idx] > 0 && counts[idx+1] > 0 && counts[idx+2] > 0 ) {
        counts[idx]--; counts[idx+1]--; counts[idx+2]--;
        res = std::min(res, calcShantenNormal(counts, melds + 1, tatsu, has_pair, idx));
        counts[idx]++; counts[idx+1]++; counts[idx+2]++;
    }

    // Try pair
    if ( !has_pair && counts[idx] >= 2 ) {
        counts[idx] -= 2;
        res = std::min(res, calcShantenNormal(counts, melds, tatsu, true, idx));
        counts[idx] += 2;
    }

    // Try tatsu (partial meld)
    if ( tatsu < 4 - melds ) {
        // Ryanmen/Penchan (edge/middle wait)
        if ( idx < 27 && idx % 9 <= 7 && counts[idx] > 0 && counts[idx+1] > 0 ) {
            counts[idx]--; counts[idx+1]--;
            res = std::min(res, calcShantenNormal(counts, melds, tatsu + 1, has_pair, idx));
            counts[idx]++; counts[idx+1]++;
        }
        // Kanchan (middle wait)
        if ( idx < 27 && idx % 9 <= 6 && counts[idx] > 0 && counts[idx+2] > 0 ) {
            counts[idx]--; counts[idx+2]--;
            res = std::min(res, calcShantenNormal(counts, melds, tatsu + 1, has_pair, idx));
            counts[idx]++; counts[idx+2]++;
        }
        // Toitsu (pair as tatsu)
        if ( counts[idx] >= 2 ) {
            counts[idx] -= 2;
            res = std::min(res, calcShantenNormal(counts, melds, tatsu + 1, has_pair, idx));
            counts[idx] += 2;
        }
    }

    // Skip this tile
    if ( counts[idx] > 0 ) {
        int tmp = counts[idx];
        counts[idx] = 0;
        res = std::min(res, calcShantenNormal(counts, melds, tatsu, has_pair, idx + 1));
        counts[idx] = tmp;
    } else {
        res = std::min(res, calcShantenNormal(counts, melds, tatsu, has_pair, idx + 1));
    }

    return res;
}

static int calcShantenChiitoitsu(const TileCounts &counts) {
    int pairs = 0, singles = 0;
    for ( int i = 0; i < 34; ++i ) {
        if ( counts[i] >= 2 ) pairs++;
        else if ( counts[i] == 1 ) singles++;
    }
    return 6 - pairs + std::max(0, 7 - pairs - singles);
}

static int calcShantenKokushi(const TileCounts &counts) {
    const int yao_tiles[] = {0, 8, 9, 17, 18, 26, 27, 28, 29, 30, 31, 32, 33};
    int yao_count = 0;
    bool has_pair = false;
    for ( int tile : yao_tiles ) {
        if ( counts[tile] >= 1 ) yao_count++;
        if ( counts[tile] >= 2 ) has_pair = true;
    }
    return 13 - yao_count - (has_pair ? 1 : 0);
}

int Hand::calcShanten() const{
    TileCounts counts = tile_counts;

    // Account for open melds
    int open_meld_count = 0;
    for ( const auto &meld : open_melds ) {
        if ( meld.type != MeldType::Pair ) open_meld_count++;
    }

    // Calculate shanten for different hand types
    int normal_shanten = calcShantenNormal(counts, open_meld_count, 0, false, 0);
    int shanten = normal_shanten;

    // Chiitoitsu and Kokushi only valid for menzen hands
    if ( is_menzen ) {
        shanten = std::min(shanten, calcShantenChiitoitsu(counts));
        shanten = std::min(shanten, calcShantenKokushi(counts));
    }

    return shanten;
}

// Hand 类的 calcFu 方法
int Hand::calcFu(const TileMeldList& melds, const TileIndex& draw, bool is_tsumo) const {
    return ::calcFu(melds, draw, round_wind, seat_wind, is_tsumo, is_menzen);
}

// 获取最佳面子组合 (用于显示和计算)
TileMeldList Hand::getBestMelds(const TileIndex& draw) const {
    HandParseResult parse_result = parseWinningHand(draw);
    if (parse_result.empty()) return {};

    // 选择翻数最高的组合
    TileMeldList best_melds;
    int max_han = -1;

    for (TileMeldList& melds : parse_result) {
        // 加入副露
        for (const auto& meld : open_melds) {
            melds.push_back(meld);
        }

        // 计算役
        YakuList yaku = calcYaku(draw, true);  // 暂时用自摸
        int han = ::calcHan(yaku, !is_menzen);

        if (han > max_han) {
            max_han = han;
            best_melds = melds;
        }
    }

    return best_melds;
}

