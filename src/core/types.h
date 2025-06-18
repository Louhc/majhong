#ifndef TYPES_H
#define TYPES_H

#include <vector>
#include <string>
#include <array>
#include <cassert>

using Tile = int; // 0-136
using TileType = int; // 0-34
using TileName = std::string;
using TileCounts = std::array<int, 34>;
using TileList = std::vector<Tile>;
using TileTypeMap = std::array<bool, 34>;
using TileTypeList = std::vector<TileType>;

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


const Tile invalid_tile = 136;
const TileType invalid_tile_type = 34;

TileTypeMap getTileTypeMap( const TileTypeList& list );

struct TileFamily {
    const int num;
    const TileTypeList list;
    const TileTypeMap map;
    TileFamily(int n, const TileTypeList& l) : num(n), list(l), map(getTileTypeMap(l)) {}
    bool contains(const Tile &tile) const;
};

struct TileMeld {
    MeldType type;
    TileType tile;
    TileMeld(MeldType t, TileType tile) : type(t), tile(tile) {}
};

using TileMeldList = std::vector<TileMeld>;
using HandParseResult = std::vector<TileMeldList>;

class Hand{
private:
    TileList hand;
    TileList chi, pon, kan, ankan;
    TileCounts tile_counts;
    bool is_menzen;
    Wind round_wind, seat_wind;
public:
    Hand(const TileList& init_tiles, Wind round, Wind seat);
    void arrangeTiles();
    TileList getAllTiles() const;
    TileCounts getAllTileCounts() const;

    bool isMenzen() const { return is_menzen; }
    Wind getRoundWind() const { return round_wind; }
    Wind getSeatWind() const { return seat_wind; }
    TileCounts getTileCounts() const { return tile_counts; };

    bool isValid() const;
    HandParseResult parseWinningHand(const Tile &draw) const;
    YakuList calcYaku(const Tile &drawnTile) const;
    int calcShanten() const;
    bool isWinningHand(const Tile &drawnTile) const;
    int calcHan() const;

    bool hasTile(const Tile &tile) const;
    bool canChi(const Tile &call) const;
    bool canPon(const Tile &call) const;
    bool canKan(const Tile &call) const;
    bool canAnkan(const Tile &tile) const;
    bool callChi(const Tile &call, const Tile &discard, int opt);
    bool callPon(const Tile &call, const Tile &discard, int opt);
    bool callKan(const Tile &call);
    bool performAnkan(const Tile &tile, const Tile &draw);
    bool drawAndDiscard(const Tile &draw, const Tile &discard);

    // 1 Han
    bool isTanyao(const Tile &draw) const;
    bool isYakuhaiSelfWind(const Tile &draw) const;
    bool isYakuhaiRoundWind(const Tile &draw) const;
    bool isYakuhaiHaku(const Tile &draw) const;
    bool isYakuhaiHatsu(const Tile &draw) const;
    bool isYakuhaiChun(const Tile &draw) const;
    // 2 Han
    bool isSankantsu(const Tile &draw) const;
    bool isHonroutou(const Tile &draw) const;
    bool isChiitoitsu(const Tile &draw) const;
    // 2 Han, Fuuro -1
    // 3 Han
    bool isHonitsu(const Tile &draw) const;
    // 6 Han, Fuuro -1
    bool isChinitsu(const Tile &draw) const;
    // Yakuman
    bool isDaisangen(const Tile &draw) const;
    bool isSuuankou(const Tile &draw) const;
    bool isTsuuiisou(const Tile &draw) const;
    bool isRyuuisou(const Tile &draw) const;
    bool isChinroutou(const Tile &draw) const;
    bool isKokushiMuso(const Tile &draw) const;
    bool isShousuushii(const Tile &draw) const;
    bool isSuukantsu(const Tile &draw) const;
    bool isChuuren(const Tile &draw) const;
    // Double Yakuman
    bool isSuuankouTanki(const Tile &draw) const;
    bool isKokushiMusoJusanmen(const Tile &draw) const;
    bool isJunseiChuuren(const Tile &draw) const;
    bool isDaisuushii(const Tile &draw) const;
};

#endif // TYPES_H