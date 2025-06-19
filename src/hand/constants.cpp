#include "constants.h"

const char* suits[] = {"m", "p", "s", "z"};

const Tile _1m = 0, _2m = 1, _3m = 2, _4m = 3, _5m = 4, _6m = 5, _7m = 6, _8m = 7, _9m = 8,
_1p = 9, _2p = 10, _3p = 11, _4p = 12, _5p = 13, _6p = 14, _7p = 15, _8p = 16, _9p = 17,
_1s = 18, _2s = 19, _3s = 20, _4s = 21, _5s = 22, _6s = 23, _7s = 24, _8s = 25, _9s = 26,
_1z = 27, _2z = 28, _3z = 29, _4z = 30, _5z = 31, _6z = 32, _7z = 33;

const Tile EastWind = 27, 
SouthWind = 28,
WestWind = 29,
NorthWind = 30,
Haku = 31,
Hatsu = 32,
Chun = 33;

const TileFamily All(34, {_1m, _2m, _3m, _4m, _5m, _6m, _7m, _8m, _9m, 
                          _1p, _2p, _3p, _4p, _5p, _6p, _7p, _8p, _9p, 
                          _1s, _2s, _3s, _4s, _5s, _6s, _7s, _8s, _9s, 
                          EastWind, SouthWind, WestWind, NorthWind, Haku, Hatsu, Chun}),
Man(9, {_1m, _2m, _3m, _4m, _5m, _6m, _7m, _8m, _9m}),
Pin(9, {_1p, _2p, _3p, _4p, _5p, _6p, _7p, _8p, _9p}),
Sou(9, {_1s, _2s, _3s, _4s, _5s, _6s, _7s, _8s, _9s}),
Kaze(4, {EastWind, SouthWind, WestWind, NorthWind}),
Sangen(3, {Haku, Hatsu, Chun}),
Honor(7, {EastWind, SouthWind, WestWind, NorthWind, Haku, Hatsu, Chun}),
Yao(13, {_1m, _9m, _1p, _9p, _1s, _9s, EastWind, SouthWind, WestWind, NorthWind, Haku, Hatsu, Chun}),
Routou(6, {_1m, _9m, _1p, _9p, _1s, _9s}),
GreenSuited(6, {_2p, _3p, _4p, _6p, _8p, Hatsu}),
Five(3, {_5m, _5p, _5s}),
SeqBegun(21, {_1m, _2m, _3m, _4m, _5m, _6m, _7m, 
              _1p, _2p, _3p, _4p, _5p, _6p, _7p, 
              _1s, _2s, _3s, _4s, _5s, _6s, _7s});