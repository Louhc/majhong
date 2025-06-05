#ifndef CONSTANTS_H
#define CONSTANTS_H

#include "types.h"

const char* suits[] = {"m", "p", "s", "z"};

const TileFamily All(
    34, 
    {0, 1, 2, 3, 4, 5, 6, 7, 8, 
     9, 10, 11, 12, 13, 14, 15, 16, 
     17, 18, 19, 20, 21, 22, 23, 24,
     25, 26, 27, 28, 29, 30, 31, 32, 33}
);

const TileFamily Yao(
    13, 
    {0, 8, 9, 17, 18, 26, 27, 28, 29, 30, 31, 32, 33}
);

const TileFamily GreenSuited(
    6, 
    {10, 11, 12, 14, 16, 32}
);

const TileFamily Routou(
    6, 
    {0, 8, 9, 17, 18, 26}
);

const TileFamily Honor(
    7, 
    {27, 28, 29, 30, 31, 32, 33}
);

const TileFamily Sangen(
    3, 
    {31, 32, 33}
);

const TileFamily Five(
    3,
    {4, 13, 22}
);

#endif // CONSTANTS_H