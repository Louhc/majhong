#include "scoring.h"
#include "constants.h"
#include <cmath>
#include <sstream>
#include <algorithm>

// 符数进位 (向10进位)
int roundUpFu(int fu) {
    return ((fu + 9) / 10) * 10;
}

// 符数计算
int calcFu(const TileMeldList& melds, const TileIndex& draw,
           Wind round_wind, Wind seat_wind, bool is_tsumo, bool is_menzen) {
    int fu = 20;  // 基本符 (副底)

    // 获取风牌 Tile
    auto getWindTile = [](Wind w) -> Tile {
        switch (w) {
            case Wind::East: return EastWind;
            case Wind::South: return SouthWind;
            case Wind::West: return WestWind;
            case Wind::North: return NorthWind;
            default: return invalid_tile;
        }
    };

    Tile round_wind_tile = getWindTile(round_wind);
    Tile seat_wind_tile = getWindTile(seat_wind);

    // 和牌方式
    if (!is_tsumo && is_menzen) {
        fu += 10;  // 门清荣和
    }

    // 自摸 (非平和)
    if (is_tsumo) {
        fu += 2;  // 自摸符
    }

    // 雀头符
    if (!melds.empty() && melds[0].type == MeldType::Pair) {
        Tile pair_tile = melds[0].tile;
        // 役牌雀头
        if (Sangen.contains(pair_tile)) fu += 2;  // 三元牌
        if (pair_tile == round_wind_tile) fu += 2;  // 场风
        if (pair_tile == seat_wind_tile) fu += 2;   // 自风
        // 连风牌 (东场东家) 只算一次
    }

    // 面子符
    for (size_t i = 1; i < melds.size(); ++i) {
        const TileMeld& meld = melds[i];
        Tile tile = meld.tile;
        bool is_yao = Yao.contains(tile);  // 幺九牌

        switch (meld.type) {
            case MeldType::ClosedTriplet:  // 暗刻
                fu += is_yao ? 8 : 4;
                break;
            case MeldType::Pon:  // 明刻
                fu += is_yao ? 4 : 2;
                break;
            case MeldType::Ankan:  // 暗杠
                fu += is_yao ? 32 : 16;
                break;
            case MeldType::Minkan:
            case MeldType::Chakan:  // 明杠
                fu += is_yao ? 16 : 8;
                break;
            case MeldType::Chi:
            case MeldType::ClosedSequence:
                // 顺子无符
                break;
            default:
                break;
        }
    }

    // 听牌类型符 (单骑、嵌张、边张)
    Tile draw_tile = draw / 4;

    // 单骑待ち (雀头听)
    if (!melds.empty() && melds[0].type == MeldType::Pair &&
        melds[0].tile == draw_tile) {
        fu += 2;
    }

    // 检查顺子中的嵌张/边张
    for (size_t i = 1; i < melds.size(); ++i) {
        const TileMeld& meld = melds[i];
        if (meld.type == MeldType::ClosedSequence || meld.type == MeldType::Chi) {
            // 嵌张 (中张待ち): 如 24 听 3
            if (draw_tile == meld.tile + 1) {
                fu += 2;
            }
            // 边张: 12 听 3 或 89 听 7
            else if ((meld.tile % 9 == 0 && draw_tile == meld.tile + 2) ||  // 12x 听 3
                     (meld.tile % 9 == 6 && draw_tile == meld.tile)) {      // x89 听 7
                fu += 2;
            }
        }
    }

    // 特殊情况: 平和自摸固定20符
    // 检查是否平和 (4顺子 + 非役牌雀头 + 两面听)
    bool is_pinfu = true;
    if (melds.size() >= 5) {
        for (size_t i = 1; i < melds.size(); ++i) {
            if (melds[i].type != MeldType::ClosedSequence) {
                is_pinfu = false;
                break;
            }
        }
        // 雀头不能是役牌
        if (is_pinfu && !melds.empty()) {
            Tile pair_tile = melds[0].tile;
            if (Sangen.contains(pair_tile) ||
                pair_tile == round_wind_tile ||
                pair_tile == seat_wind_tile) {
                is_pinfu = false;
            }
        }
    } else {
        is_pinfu = false;
    }

    if (is_pinfu && is_tsumo && is_menzen) {
        return 20;  // 平和自摸固定20符
    }

    // 平和荣和固定30符
    if (is_pinfu && !is_tsumo && is_menzen) {
        return 30;
    }

    // 副露平和型固定30符
    if (!is_menzen && fu == 20) {
        return 30;  // 鳴き平和形 (喰い平和)
    }

    return roundUpFu(fu);
}

// 基本点数计算
int calcBasePoints(int han, int fu) {
    if (han >= 13) {
        return 8000;  // 役满
    } else if (han >= 11) {
        return 6000;  // 三倍满
    } else if (han >= 8) {
        return 4000;  // 倍满
    } else if (han >= 6) {
        return 3000;  // 跳满
    } else if (han >= 5) {
        return 2000;  // 满贯
    } else if (han >= 4) {
        if (fu >= 40) return 2000;  // 满贯
        if (fu >= 30) {
            int base = fu * (1 << (2 + han));
            return std::min(base, 2000);  // 切上满贯
        }
    } else if (han >= 3) {
        if (fu >= 70) return 2000;  // 满贯
        if (fu >= 60) {
            int base = fu * (1 << (2 + han));
            return std::min(base, 2000);
        }
    }

    // 普通计算: fu * 2^(2+han)
    int base = fu * (1 << (2 + han));
    return std::min(base, 2000);  // 切上满贯
}

// 进位到100
static int roundUp100(int points) {
    return ((points + 99) / 100) * 100;
}

// 最终得点计算
AgariResult calcScore(int han, int fu, bool is_dealer, bool is_tsumo) {
    AgariResult result;
    result.han = han;
    result.fu = fu;
    result.is_dealer = is_dealer;
    result.is_tsumo = is_tsumo;
    result.base_points = calcBasePoints(han, fu);

    int base = result.base_points;

    if (is_dealer) {
        if (is_tsumo) {
            // 庄家自摸: 各家支付 base * 2
            int each = roundUp100(base * 2);
            result.total_points = each * 3;
        } else {
            // 庄家荣和: 放铳者支付 base * 6
            result.total_points = roundUp100(base * 6);
        }
    } else {
        if (is_tsumo) {
            // 闲家自摸: 庄家支付 base * 2, 闲家各支付 base * 1
            int dealer_pay = roundUp100(base * 2);
            int other_pay = roundUp100(base);
            result.total_points = dealer_pay + other_pay * 2;
        } else {
            // 闲家荣和: 放铳者支付 base * 4
            result.total_points = roundUp100(base * 4);
        }
    }

    return result;
}

// 判断是否满贯以上
bool isMangan(int han, int fu) {
    return calcBasePoints(han, fu) >= 2000;
}

// 获取役名
std::string getYakuName(Yaku yaku) {
    switch (yaku) {
        // 1翻
        case Yaku::Richii: return "立直";
        case Yaku::Tanyao: return "断幺九";
        case Yaku::Tsumo: return "门清自摸";
        case Yaku::YakuhaiSelfWind: return "自风";
        case Yaku::YakuhaiRoundWind: return "场风";
        case Yaku::YakuhaiHaku: return "役牌 白";
        case Yaku::YakuhaiHatsu: return "役牌 发";
        case Yaku::YakuhaiChun: return "役牌 中";
        case Yaku::Pinfu: return "平和";
        case Yaku::Iipeikou: return "一杯口";
        case Yaku::Chankan: return "抢杠";
        case Yaku::Rinshan: return "岭上开花";
        case Yaku::Haitei: return "海底摸月";
        case Yaku::Houtei: return "河底捞鱼";
        case Yaku::Ippatsu: return "一发";

        // 2翻
        case Yaku::DoubleRichii: return "两立直";
        case Yaku::SanshokuDoukou: return "三色同刻";
        case Yaku::Sankantsu: return "三杠子";
        case Yaku::Toitoi: return "对对和";
        case Yaku::Sanankou: return "三暗刻";
        case Yaku::Shousangen: return "小三元";
        case Yaku::Honroutou: return "混老头";
        case Yaku::Chiitoitsu: return "七对子";
        case Yaku::Honchan: return "混全带幺九";
        case Yaku::Ittsuu: return "一气通贯";
        case Yaku::Sanshoku: return "三色同顺";

        // 3翻
        case Yaku::Ryanpeikou: return "二杯口";
        case Yaku::Junchan: return "纯全带幺九";
        case Yaku::Honitsu: return "混一色";

        // 6翻
        case Yaku::Chinitsu: return "清一色";

        // 役满
        case Yaku::Daisangen: return "大三元";
        case Yaku::Suuankou: return "四暗刻";
        case Yaku::Tsuuiisou: return "字一色";
        case Yaku::Ryuuisou: return "绿一色";
        case Yaku::Chinroutou: return "清老头";
        case Yaku::KokushiMuso: return "国士无双";
        case Yaku::Shousuushii: return "小四喜";
        case Yaku::Suukantsu: return "四杠子";
        case Yaku::Chuuren: return "九莲宝灯";

        // 双倍役满
        case Yaku::SuuankouTanki: return "四暗刻单骑";
        case Yaku::KokushiMusoJusanmen: return "国士无双十三面";
        case Yaku::JunseiChuuren: return "纯正九莲宝灯";
        case Yaku::Daisuushii: return "大四喜";

        default: return "未知役";
    }
}

std::string getYakuNames(const YakuList& yaku_list) {
    std::ostringstream ss;
    for (size_t i = 0; i < yaku_list.size(); ++i) {
        if (i > 0) ss << " ";
        ss << getYakuName(yaku_list[i]);
    }
    return ss.str();
}
