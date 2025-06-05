#ifndef TYPES_H
#define TYPES_H

#include <vector>
#include <string>
#include <array>

#include "tiles.h"

using Tile = int; // 0-136
using TileType = int; // 0-34
using TileName = std::string;
using TileCounts = std::array<int, 34>;
using TileList = std::vector<Tile>;
using TileTypeMap = std::array<bool, 34>;
using TileTypeList = std::vector<TileType>;
enum class Wind { East, South, West, North };
const Tile invalid_tile = 136;
const TileType invalid_tile_type = 34;

TileTypeMap getTileTypeMap( const TileTypeList& list ) {
    TileTypeMap map; map.fill(false);
    for ( const TileType& tile_type : list ) 
        map[tile_type] = true;
    return map;
}

struct TileFamily {
    const int num;
    const TileTypeList list;
    const TileTypeMap map;
    TileFamily(int n, const TileTypeList& l) : num(n), list(l), map(getTileTypeMap(l)) {}
    bool contains(const Tile &tile) const;
};

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

    bool isMenzen() const { return is_menzen; }
    Wind getRoundWind() const { return round_wind; }
    Wind getSeatWind() const { return seat_wind; }
    TileCounts getTileCounts() const { return tile_counts; };

    bool isValid() const;
    int calcShanten() const;
    bool isWinningHand(Tile drawnTile) const;
    int calcHan() const;

    bool hasTile(const Tile &tile) const{
        assert(isValidTile(tile));
        return tile_counts[getTileType(tile)] > 0;
    }
    bool canChi(const Tile &call) const;
    bool canPon(const Tile &call) const;
    bool canKan(const Tile &call) const;
    bool canAnkan(const Tile &tile) const;
    bool callChi(const Tile &call, const Tile &discard, int opt);
    bool callPon(const Tile &call, const Tile &discard, int opt);
    bool callKan(const Tile &call);
    bool performAnkan(const Tile &tile, const Tile &draw);
    bool drawAndDiscard(Tile draw, Tile discard);
    
    // 1 Han
    bool isTanyao(Tile drawnTile) const;
    bool isYakuhaiSelfWind(Tile drawnTile) const;
    bool isYakuhaiRoundWind(Tile drawnTile) const;
    bool isYakuhaiChun(Tile drawnTile) const;
    bool isYakuhaiHatsu(Tile drawnTile) const;
    bool isYakuhaiHaku(Tile drawnTile) const;
    bool isPinfu(Tile drawnTile) const;
    bool isIipeikou(Tile drawnTile) const;
    // 2 Han
    bool isSanshokuDoukou(Tile drawnTile) const;
    bool isSankantsu(Tile drawnTile) const;
    bool isToitoi(Tile drawnTile) const;
    bool isSanankou(Tile drawnTile) const;
    bool isShousangen(Tile drawnTile) const;
    bool isHonroutou(Tile drawnTile) const;
    bool isChiitoitsu(Tile drawnTile) const;
    // 2 Han, Fuuro -1
    bool isHonchan(Tile drawnTile) const;
    bool isIttsuu(Tile drawnTile) const;
    bool isSanshoku(Tile drawnTile) const;
    // 3 Han
    bool isRyanpeikou(Tile drawnTile) const;
    bool isJunchan(Tile drawnTile) const;
    bool isHonitsu(Tile drawnTile) const;
    // 6 Han, Fuuro -1
    bool isChinitsu(Tile drawnTile) const;
    // Yakuman
    bool isDaisangen(Tile drawnTile) const;
    bool isSuuankou(Tile drawnTile) const;
    bool isTsuuiisou(Tile drawnTile) const;
    bool isRyuuisou(Tile drawnTile) const;
    bool isChinroutou(Tile drawnTile) const;
    bool isKokushiMuso(Tile drawnTile) const;
    bool isShousuushii(Tile drawnTile) const;
    bool isSuukantsu(Tile drawnTile) const;
    bool isChuuren(Tile drawnTile) const;
    // Double Yakuman
    bool isSuuankouTanki(Tile drawnTile) const;
    bool isKokushiMusoJusanmen(Tile drawnTile) const;
    bool isJunseiChuuren(Tile drawnTile) const;
    bool isDaisuushii(Tile drawnTile) const;
};

#endif // TYPES_H