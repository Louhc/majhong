#include <iostream>
#include <cassert>
#include <algorithm>
#include "types.h"
#include "constants.h"
#include "tiles.h"
#include "printer.h"

// Test helper macros
#define TEST_ASSERT(cond, msg) \
    if (!(cond)) { \
        std::cerr << "FAILED: " << msg << std::endl; \
        return 1; \
    } else { \
        std::cout << "PASSED: " << msg << std::endl; \
    }

#define TEST_YAKU(hand, draw, yaku, expected, msg) \
    { \
        YakuList yaku_list = hand.calcYaku(draw, true); \
        bool found = std::find(yaku_list.begin(), yaku_list.end(), yaku) != yaku_list.end(); \
        TEST_ASSERT(found == expected, msg); \
    }

// TileIndex helper: tile * 4 + instance (0-3)
inline TileIndex TI(Tile tile, int instance = 0) { return tile * 4 + instance; }

// Test Tanyao (All Simples)
int testTanyao() {
    std::cout << "\n=== Testing Tanyao ===" << std::endl;

    // Tanyao hand: 2m 3m 4m 5m 6m 7m 2p 3p 4p 5p 6p 7p 8p (waiting on 5p or 8p)
    Hand hand1({TI(_2m), TI(_3m), TI(_4m), TI(_5m), TI(_6m), TI(_7m),
                TI(_2p), TI(_3p), TI(_4p), TI(_5p), TI(_6p), TI(_7p), TI(_8p)}, Wind::East, Wind::East);
    TEST_ASSERT(hand1.isTanyao(TI(_5p, 1)), "isTanyao with all simples");

    // Not Tanyao - has terminal (1m)
    Hand hand2({TI(_1m), TI(_2m), TI(_3m), TI(_4m), TI(_5m), TI(_6m),
                TI(_2p), TI(_3p), TI(_4p), TI(_5p), TI(_6p), TI(_7p), TI(_8p)}, Wind::East, Wind::East);
    TEST_ASSERT(!hand2.isTanyao(TI(_5p, 1)), "not Tanyao with terminal");

    return 0;
}

// Test Yakuhai (Value Tiles)
int testYakuhai() {
    std::cout << "\n=== Testing Yakuhai ===" << std::endl;

    // Hand with 3 Haku: Haku Haku Haku 1m 2m 3m 4m 5m 6m 7m 8m 9m 1p
    Hand hand1({TI(Haku), TI(Haku, 1), TI(Haku, 2), TI(_1m), TI(_2m), TI(_3m),
                TI(_4m), TI(_5m), TI(_6m), TI(_7m), TI(_8m), TI(_9m), TI(_1p)}, Wind::East, Wind::East);
    TEST_ASSERT(hand1.isYakuhaiHaku(TI(_1p, 1)), "isYakuhaiHaku with 3 Haku");

    // Hand with 3 Hatsu
    Hand hand2({TI(Hatsu), TI(Hatsu, 1), TI(Hatsu, 2), TI(_1m), TI(_2m), TI(_3m),
                TI(_4m), TI(_5m), TI(_6m), TI(_7m), TI(_8m), TI(_9m), TI(_1p)}, Wind::East, Wind::East);
    TEST_ASSERT(hand2.isYakuhaiHatsu(TI(_1p, 1)), "isYakuhaiHatsu with 3 Hatsu");

    // Hand with 3 Chun
    Hand hand3({TI(Chun), TI(Chun, 1), TI(Chun, 2), TI(_1m), TI(_2m), TI(_3m),
                TI(_4m), TI(_5m), TI(_6m), TI(_7m), TI(_8m), TI(_9m), TI(_1p)}, Wind::East, Wind::East);
    TEST_ASSERT(hand3.isYakuhaiChun(TI(_1p, 1)), "isYakuhaiChun with 3 Chun");

    // Test seat wind (East)
    Hand hand4({TI(EastWind), TI(EastWind, 1), TI(EastWind, 2), TI(_1m), TI(_2m), TI(_3m),
                TI(_4m), TI(_5m), TI(_6m), TI(_7m), TI(_8m), TI(_9m), TI(_1p)}, Wind::East, Wind::East);
    TEST_ASSERT(hand4.isYakuhaiSelfWind(TI(_1p, 1)), "isYakuhaiSelfWind East");
    TEST_ASSERT(hand4.isYakuhaiRoundWind(TI(_1p, 1)), "isYakuhaiRoundWind East");

    return 0;
}

// Test Chiitoitsu (7 Pairs)
int testChiitoitsu() {
    std::cout << "\n=== Testing Chiitoitsu ===" << std::endl;

    // 7 pairs: 1m1m 2m2m 3m3m 4m4m 5m5m 6m6m 7m (waiting on 7m)
    Hand hand1({TI(_1m), TI(_1m, 1), TI(_2m), TI(_2m, 1), TI(_3m), TI(_3m, 1),
                TI(_4m), TI(_4m, 1), TI(_5m), TI(_5m, 1), TI(_6m), TI(_6m, 1), TI(_7m)}, Wind::East, Wind::East);
    TEST_ASSERT(hand1.isChiitoitsu(TI(_7m, 1)), "isChiitoitsu with 7 pairs");

    // Not Chiitoitsu - only 6 pairs
    Hand hand2({TI(_1m), TI(_1m, 1), TI(_2m), TI(_2m, 1), TI(_3m), TI(_3m, 1),
                TI(_4m), TI(_4m, 1), TI(_5m), TI(_5m, 1), TI(_6m), TI(_7m), TI(_8m)}, Wind::East, Wind::East);
    TEST_ASSERT(!hand2.isChiitoitsu(TI(_6m, 1)), "not Chiitoitsu with only 6 pairs");

    return 0;
}

// Test Honitsu (Half Flush)
int testHonitsu() {
    std::cout << "\n=== Testing Honitsu ===" << std::endl;

    // Honitsu: All manzu + honors
    Hand hand1({TI(_1m), TI(_2m), TI(_3m), TI(_4m), TI(_5m), TI(_6m),
                TI(_7m), TI(_8m), TI(_9m), TI(EastWind), TI(EastWind, 1), TI(EastWind, 2), TI(_1m, 1)}, Wind::East, Wind::East);
    TEST_ASSERT(hand1.isHonitsu(TI(_1m, 2)), "isHonitsu with manzu + honors");

    // Not Honitsu - mixed suits
    Hand hand2({TI(_1m), TI(_2m), TI(_3m), TI(_4m), TI(_5m), TI(_6m),
                TI(_1p), TI(_2p), TI(_3p), TI(_4p), TI(_5p), TI(_6p), TI(_7p)}, Wind::East, Wind::East);
    TEST_ASSERT(!hand2.isHonitsu(TI(_7p, 1)), "not Honitsu with mixed suits");

    return 0;
}

// Test Chinitsu (Full Flush)
int testChinitsu() {
    std::cout << "\n=== Testing Chinitsu ===" << std::endl;

    // Chinitsu: All manzu
    Hand hand1({TI(_1m), TI(_2m), TI(_3m), TI(_4m), TI(_5m), TI(_6m),
                TI(_7m), TI(_8m), TI(_9m), TI(_1m, 1), TI(_2m, 1), TI(_3m, 1), TI(_4m, 1)}, Wind::East, Wind::East);
    TEST_ASSERT(hand1.isChinitsu(TI(_4m, 2)), "isChinitsu with all manzu");

    // Not Chinitsu - has honors
    Hand hand2({TI(_1m), TI(_2m), TI(_3m), TI(_4m), TI(_5m), TI(_6m),
                TI(_7m), TI(_8m), TI(_9m), TI(EastWind), TI(EastWind, 1), TI(EastWind, 2), TI(_1m, 1)}, Wind::East, Wind::East);
    TEST_ASSERT(!hand2.isChinitsu(TI(_1m, 2)), "not Chinitsu with honors");

    return 0;
}

// Test Honroutou (All Terminals and Honors)
int testHonroutou() {
    std::cout << "\n=== Testing Honroutou ===" << std::endl;

    // Honroutou: All terminals and honors
    Hand hand1({TI(_1m), TI(_1m, 1), TI(_9m), TI(_9m, 1), TI(_1p), TI(_1p, 1),
                TI(_9p), TI(_9p, 1), TI(_1s), TI(_1s, 1), TI(EastWind), TI(EastWind, 1), TI(EastWind, 2)}, Wind::East, Wind::East);
    TEST_ASSERT(hand1.isHonroutou(TI(_1m, 2)), "isHonroutou");

    return 0;
}

// Test Kokushi Muso (Thirteen Orphans)
int testKokushiMuso() {
    std::cout << "\n=== Testing Kokushi Muso ===" << std::endl;

    // Kokushi: All 13 different terminals and honors, waiting on 1 more
    Hand hand1({TI(_1m), TI(_9m), TI(_1p), TI(_9p), TI(_1s), TI(_9s),
                TI(EastWind), TI(SouthWind), TI(WestWind), TI(NorthWind), TI(Haku), TI(Hatsu), TI(Chun)}, Wind::East, Wind::East);
    TEST_ASSERT(hand1.isKokushiMuso(TI(_1m, 1)), "isKokushiMuso");

    // 13-sided wait Kokushi
    TEST_ASSERT(hand1.isKokushiMusoJusanmen(TI(_1m, 1)), "isKokushiMusoJusanmen");

    return 0;
}

// Test Daisangen (Big Three Dragons)
int testDaisangen() {
    std::cout << "\n=== Testing Daisangen ===" << std::endl;

    // Daisangen: 3 of each dragon
    Hand hand1({TI(Haku), TI(Haku, 1), TI(Haku, 2), TI(Hatsu), TI(Hatsu, 1), TI(Hatsu, 2),
                TI(Chun), TI(Chun, 1), TI(Chun, 2), TI(_1m), TI(_2m), TI(_3m), TI(_4m)}, Wind::East, Wind::East);
    TEST_ASSERT(hand1.isDaisangen(TI(_4m, 1)), "isDaisangen");

    return 0;
}

// Test Tsuuiisou (All Honors)
int testTsuuiisou() {
    std::cout << "\n=== Testing Tsuuiisou ===" << std::endl;

    // Tsuuiisou: All honor tiles
    Hand hand1({TI(EastWind), TI(EastWind, 1), TI(EastWind, 2), TI(SouthWind), TI(SouthWind, 1), TI(SouthWind, 2),
                TI(WestWind), TI(WestWind, 1), TI(WestWind, 2), TI(NorthWind), TI(NorthWind, 1), TI(Haku), TI(Haku, 1)}, Wind::East, Wind::East);
    TEST_ASSERT(hand1.isTsuuiisou(TI(Haku, 2)), "isTsuuiisou");

    return 0;
}

// Test Ryuuisou (All Green)
int testRyuuisou() {
    std::cout << "\n=== Testing Ryuuisou ===" << std::endl;

    // Ryuuisou: All green tiles (2s, 3s, 4s, 6s, 8s, Hatsu)
    Hand hand1({TI(_2s), TI(_3s), TI(_4s), TI(_2s, 1), TI(_3s, 1), TI(_4s, 1),
                TI(_6s), TI(_6s, 1), TI(_6s, 2), TI(_8s), TI(_8s, 1), TI(Hatsu), TI(Hatsu, 1)}, Wind::East, Wind::East);
    TEST_ASSERT(hand1.isRyuuisou(TI(Hatsu, 2)), "isRyuuisou");

    return 0;
}

// Test Chinroutou (All Terminals)
int testChinroutou() {
    std::cout << "\n=== Testing Chinroutou ===" << std::endl;

    // Chinroutou: All terminal tiles
    Hand hand1({TI(_1m), TI(_1m, 1), TI(_1m, 2), TI(_9m), TI(_9m, 1), TI(_9m, 2),
                TI(_1p), TI(_1p, 1), TI(_1p, 2), TI(_9p), TI(_9p, 1), TI(_1s), TI(_1s, 1)}, Wind::East, Wind::East);
    TEST_ASSERT(hand1.isChinroutou(TI(_1s, 2)), "isChinroutou");

    return 0;
}

// Test Shousuushii (Little Four Winds)
int testShousuushii() {
    std::cout << "\n=== Testing Shousuushii ===" << std::endl;

    // Shousuushii: 3 triplets + 1 pair of winds
    Hand hand1({TI(EastWind), TI(EastWind, 1), TI(EastWind, 2), TI(SouthWind), TI(SouthWind, 1), TI(SouthWind, 2),
                TI(WestWind), TI(WestWind, 1), TI(WestWind, 2), TI(NorthWind), TI(NorthWind, 1), TI(_1m), TI(_2m)}, Wind::East, Wind::East);
    TEST_ASSERT(hand1.isShousuushii(TI(_3m)), "isShousuushii");

    return 0;
}

// Test Daisuushii (Big Four Winds)
int testDaisuushii() {
    std::cout << "\n=== Testing Daisuushii ===" << std::endl;

    // Daisuushii: 4 triplets of winds
    Hand hand1({TI(EastWind), TI(EastWind, 1), TI(EastWind, 2), TI(SouthWind), TI(SouthWind, 1), TI(SouthWind, 2),
                TI(WestWind), TI(WestWind, 1), TI(WestWind, 2), TI(NorthWind), TI(NorthWind, 1), TI(NorthWind, 2), TI(_1m)}, Wind::East, Wind::East);
    TEST_ASSERT(hand1.isDaisuushii(TI(_1m, 1)), "isDaisuushii");

    return 0;
}

// Test isWinningHand
int testIsWinningHand() {
    std::cout << "\n=== Testing isWinningHand ===" << std::endl;

    // Winning hand: 1m2m3m 4m5m6m 7m8m9m 1p1p1p 2p (waiting on 2p)
    Hand hand1({TI(_1m), TI(_2m), TI(_3m), TI(_4m), TI(_5m), TI(_6m),
                TI(_7m), TI(_8m), TI(_9m), TI(_1p), TI(_1p, 1), TI(_1p, 2), TI(_2p)}, Wind::East, Wind::East);
    TEST_ASSERT(hand1.isWinningHand(TI(_2p, 1)), "isWinningHand standard form");

    // Kokushi winning hand
    Hand hand2({TI(_1m), TI(_9m), TI(_1p), TI(_9p), TI(_1s), TI(_9s),
                TI(EastWind), TI(SouthWind), TI(WestWind), TI(NorthWind), TI(Haku), TI(Hatsu), TI(Chun)}, Wind::East, Wind::East);
    TEST_ASSERT(hand2.isWinningHand(TI(_1m, 1)), "isWinningHand Kokushi");

    // Chiitoitsu winning hand
    Hand hand3({TI(_1m), TI(_1m, 1), TI(_2m), TI(_2m, 1), TI(_3m), TI(_3m, 1),
                TI(_4m), TI(_4m, 1), TI(_5m), TI(_5m, 1), TI(_6m), TI(_6m, 1), TI(_7m)}, Wind::East, Wind::East);
    TEST_ASSERT(hand3.isWinningHand(TI(_7m, 1)), "isWinningHand Chiitoitsu");

    // Not winning hand
    Hand hand4({TI(_1m), TI(_2m), TI(_3m), TI(_4m), TI(_5m), TI(_6m),
                TI(_7m), TI(_8m), TI(_9m), TI(_1p), TI(_2p), TI(_3p), TI(_5p)}, Wind::East, Wind::East);
    TEST_ASSERT(!hand4.isWinningHand(TI(_6p)), "not winning hand - no pair");

    return 0;
}

// Test calcShanten
int testCalcShanten() {
    std::cout << "\n=== Testing calcShanten ===" << std::endl;

    // Tenpai hand (shanten = -1 means waiting, but we return 0 for tenpai)
    Hand hand1({TI(_1m), TI(_2m), TI(_3m), TI(_4m), TI(_5m), TI(_6m),
                TI(_7m), TI(_8m), TI(_9m), TI(_1p), TI(_1p, 1), TI(_1p, 2), TI(_2p)}, Wind::East, Wind::East);
    int shanten1 = hand1.calcShanten();
    std::cout << "Tenpai hand shanten: " << shanten1 << std::endl;
    TEST_ASSERT(shanten1 == 0, "calcShanten tenpai hand should be 0");

    // 1-shanten hand
    Hand hand2({TI(_1m), TI(_2m), TI(_3m), TI(_4m), TI(_5m), TI(_6m),
                TI(_7m), TI(_8m), TI(_9m), TI(_1p), TI(_1p, 1), TI(_3p), TI(_5p)}, Wind::East, Wind::East);
    int shanten2 = hand2.calcShanten();
    std::cout << "1-shanten hand shanten: " << shanten2 << std::endl;
    TEST_ASSERT(shanten2 >= 0 && shanten2 <= 2, "calcShanten 1-shanten hand");

    // Kokushi tenpai
    Hand hand3({TI(_1m), TI(_9m), TI(_1p), TI(_9p), TI(_1s), TI(_9s),
                TI(EastWind), TI(SouthWind), TI(WestWind), TI(NorthWind), TI(Haku), TI(Hatsu), TI(Chun)}, Wind::East, Wind::East);
    int shanten3 = hand3.calcShanten();
    std::cout << "Kokushi tenpai shanten: " << shanten3 << std::endl;
    TEST_ASSERT(shanten3 == 0, "calcShanten Kokushi tenpai");

    // Chiitoitsu tenpai
    Hand hand4({TI(_1m), TI(_1m, 1), TI(_2m), TI(_2m, 1), TI(_3m), TI(_3m, 1),
                TI(_4m), TI(_4m, 1), TI(_5m), TI(_5m, 1), TI(_6m), TI(_6m, 1), TI(_7m)}, Wind::East, Wind::East);
    int shanten4 = hand4.calcShanten();
    std::cout << "Chiitoitsu tenpai shanten: " << shanten4 << std::endl;
    TEST_ASSERT(shanten4 == 0, "calcShanten Chiitoitsu tenpai");

    return 0;
}

int main() {
    int failed = 0;

    failed += testTanyao();
    failed += testYakuhai();
    failed += testChiitoitsu();
    failed += testHonitsu();
    failed += testChinitsu();
    failed += testHonroutou();
    failed += testKokushiMuso();
    failed += testDaisangen();
    failed += testTsuuiisou();
    failed += testRyuuisou();
    failed += testChinroutou();
    failed += testShousuushii();
    failed += testDaisuushii();
    failed += testIsWinningHand();
    failed += testCalcShanten();

    std::cout << "\n=== Test Summary ===" << std::endl;
    if (failed == 0) {
        std::cout << "All Yaku tests passed!" << std::endl;
    } else {
        std::cout << failed << " test(s) failed!" << std::endl;
    }

    return failed;
}
