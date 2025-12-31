#ifndef SCORING_H
#define SCORING_H

#include "types.h"

// 和牌结果
struct AgariResult {
    YakuList yaku;
    int han;
    int fu;
    int base_points;
    int total_points;
    bool is_dealer;
    bool is_tsumo;
    std::string yaku_names;
};

// 符数计算
int calcFu(const TileMeldList& melds, const TileIndex& draw,
           Wind round_wind, Wind seat_wind, bool is_tsumo, bool is_menzen);

// 基本点数计算 (切上满贯)
int calcBasePoints(int han, int fu);

// 最终得点计算
// 返回: 庄家自摸时为各家支付，闲家自摸时为庄家/闲家支付，荣和时为放铳者支付
AgariResult calcScore(int han, int fu, bool is_dealer, bool is_tsumo);

// 符数进位 (向10进位)
int roundUpFu(int fu);

// 获取役名
std::string getYakuName(Yaku yaku);
std::string getYakuNames(const YakuList& yaku_list);

// 判断是否满贯以上
bool isMangan(int han, int fu);

#endif // SCORING_H
