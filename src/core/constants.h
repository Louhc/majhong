#ifndef CONSTANTS_H
#define CONSTANTS_H

#include "types.h"

// 牌的花色
extern const char* suits[4];

// 万子(1m-9m)
extern const Tile _1m, _2m, _3m, _4m, _5m, _6m, _7m, _8m, _9m;
// 筒子(1p-9p)
extern const Tile _1p, _2p, _3p, _4p, _5p, _6p, _7p, _8p, _9p;
// 索子(1s-9s)
extern const Tile _1s, _2s, _3s, _4s, _5s, _6s, _7s, _8s, _9s;
// 字牌(1z-7z)
extern const Tile _1z, _2z, _3z, _4z, _5z, _6z, _7z;

// 风牌和三元牌别名
extern const Tile EastWind, SouthWind, WestWind, NorthWind;
extern const Tile Haku, Hatsu, Chun;

// 牌组
extern const TileFamily All, Man, Pin, Sou, Kaze, Sangen, Honor, Yao, Routou, GreenSuited, Five, SeqBegun;

#endif // CONSTANTS_H