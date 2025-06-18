#ifndef TYPES_H
#define TYPES_H

#include <vector>
#include <string>
#include <array>
#include <cassert>

using Tile = int; // 0-34
using TileIndex = int; // 0-136
using TileName = std::string;
using TileCounts = std::array<int, 35>;
using TileIndexList = std::vector<TileIndex>;
using TileMap = std::array<bool, 35>;
using TileList = std::vector<Tile>;

enum class Wind { East, South, West, North };
enum class Yaku { Richii, Tanyao, Tsumo, YakuhaiSelfWind, YakuhaiRoundWind, YakuhaiHaku, YakuhaiHatsu, YakuhaiChun,
                Pinfu, Iipeikou, Chankan, Rinshan, Haitei, Houtei, Ippatsu, // 1 han
                DoubleRichii, SanshokuDoukou, Sankantsu, Toitoi, Sanankou, Shousangen, Honroutou, Chiitoitsu, // 2 han
                Honchan, Ittsuu, Sanshoku, // 2 han - 1
                Ryanpeikou, // 3 han
                Junchan, Honitsu, // 3 han - 1
                Chinitsu, // 6 han - 1
                Daisangen, Suuankou, Tsuuiisou, Ryuuisou, Chinroutou, KokushiMuso, Shousuushii, Suukantsu, Chuuren, 
                SuuankouTanki, KokushiMusoJusanmen, JunseiChuuren, Daisuushii // Yakuman
};
using YakuList = std::vector<Yaku>;

enum class MeldType {
    Ankan,   // 暗杠
    Minkan,  // 明杠 (大明杠)
    Chakan,  // 加杠 (小明杠)
    Chi,     // 吃 (明吃)
    Pon,     // 碰 (明碰)
    ClosedSequence, // 闭合顺子 (和牌时手牌中的顺子)
    ClosedTriplet,  // 闭合刻子 (和牌时手牌中的刻子)
    Pair     // 雀头
};


const TileIndex invalid_tile_index = 136;
const Tile invalid_tile = 34;

TileMap getTileMap( const TileList& list );

struct TileFamily {
    const int num;
    const TileList list;
    const TileMap map;
    TileFamily(int n, const TileList& l) : num(n), list(l), map(getTileMap(l)) {}
    bool containsIdx(const TileIndex &tile_index) const;
    bool contains(const Tile &tile) const;
};

struct TileMeld {
    MeldType type; Tile tile;
    TileMeld(const MeldType &t, const Tile &tile) : type(t), tile(tile) {}
};

using TileMeldList = std::vector<TileMeld>;
using HandParseResult = std::vector<TileMeldList>;

class Hand{
private:
    TileIndexList hand;
    TileIndexList open;
    TileMeldList open_melds;
    TileCounts tile_counts;
    Wind round_wind, seat_wind;
    int is_menzen, is_richii; 
    // is_richii = 1 : ippatsu
    // is_richii > 1 : not ippatsu
public:
    Hand(const TileList& init_tiles, Wind round, Wind seat);
    void arrangeTiles();
    TileList getAllTiles() const;
    TileCounts getAllTileCounts() const;

    bool isMenzen() const { return is_menzen; }
    Wind getRoundWind() const { return round_wind; }
    Wind getSeatWind() const { return seat_wind; }
    TileCounts getTileCounts() const { return tile_counts; };

    bool canChi(const TileIndex &call) const;
    bool canPon(const TileIndex &call) const;
    bool canKan(const TileIndex &call) const;
    bool canAnkan(const TileIndex &tile_index) const;
    bool callChi(const TileIndex &call, const TileIndex &discard, int opt);
    bool callPon(const TileIndex &call, const TileIndex &discard, int opt);
    bool callKan(const TileIndex &call);
    bool performAnkan(const TileIndex &tile_index);
    bool performChakan(const TileIndex &tile_index);
    bool drawAndDiscard(const TileIndex &draw, const TileIndex &discard);

    HandParseResult parseWinningHand(const TileIndex &draw) const;
    YakuList calcYaku(const TileIndex &draw) const;
    int calcShanten() const;
    bool isWinningHand(const TileIndex &draw) const;
    int calcHan() const;

    bool isTanyao(const TileIndex &draw) const;
    bool isYakuhaiSelfWind(const TileIndex &draw) const;
    bool isYakuhaiRoundWind(const TileIndex &draw) const;
    bool isYakuhaiHaku(const TileIndex &draw) const;
    bool isYakuhaiHatsu(const TileIndex &draw) const;
    bool isYakuhaiChun(const TileIndex &draw) const;
    bool isSankantsu(const TileIndex &draw) const;
    bool isHonroutou(const TileIndex &draw) const;
    bool isChiitoitsu(const TileIndex &draw) const;
    bool isHonitsu(const TileIndex &draw) const;
    bool isChinitsu(const TileIndex &draw) const;
    bool isDaisangen(const TileIndex &draw) const;
    bool isSuuankou(const TileIndex &draw) const;
    bool isTsuuiisou(const TileIndex &draw) const;
    bool isRyuuisou(const TileIndex &draw) const;
    bool isChinroutou(const TileIndex &draw) const;
    bool isKokushiMuso(const TileIndex &draw) const;
    bool isShousuushii(const TileIndex &draw) const;
    bool isSuukantsu(const TileIndex &draw) const;
    bool isChuuren(const TileIndex &draw) const;
    bool isSuuankouTanki(const TileIndex &draw) const;
    bool isKokushiMusoJusanmen(const TileIndex &draw) const;
    bool isJunseiChuuren(const TileIndex &draw) const;
    bool isDaisuushii(const TileIndex &draw) const;
};

#endif // TYPES_H