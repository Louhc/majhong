#include <iostream>
#include <cassert>
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

// Test canChi functionality
int testCanChi() {
    std::cout << "\n=== Testing canChi ===" << std::endl;

    // Hand: 1m 2m 3m 4m 5m 6m 7m 8m 9m 1p 2p 3p 4p
    Hand hand1({0, 4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48}, Wind::East, Wind::East);

    // Can chi 3m (tile 8) with 1m 2m
    TEST_ASSERT(hand1.canChi(8), "canChi 3m with 1m 2m");

    // Can chi 5m (tile 16) with 4m 6m
    TEST_ASSERT(hand1.canChi(16), "canChi 5m with 4m 6m");

    // Hand with honor tiles - cannot chi honor tiles
    Hand hand2({108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120}, Wind::East, Wind::East);
    TEST_ASSERT(!hand2.canChi(120), "cannot chi honor tiles");

    return 0;
}

// Test canPon functionality
int testCanPon() {
    std::cout << "\n=== Testing canPon ===" << std::endl;

    // Hand with pairs: 1m 1m 2m 2m 3m 3m 4m 4m 5m 5m 6m 6m 7m
    Hand hand1({0, 1, 4, 5, 8, 9, 12, 13, 16, 17, 20, 21, 24}, Wind::East, Wind::East);

    // Can pon 1m (need 2 in hand to pon with 1 called)
    TEST_ASSERT(hand1.canPon(2), "canPon 1m when have 2 in hand");

    // Hand with triplet: 1m 1m 1m 2m 3m 4m 5m 6m 7m 8m 9m 1p 2p
    Hand hand2({0, 1, 2, 4, 8, 12, 16, 20, 24, 28, 32, 36, 40}, Wind::East, Wind::East);

    // Cannot pon 2m (only have 1)
    TEST_ASSERT(!hand2.canPon(4), "cannot pon 2m when only have 1");

    return 0;
}

// Test canKan functionality
int testCanKan() {
    std::cout << "\n=== Testing canKan ===" << std::endl;

    // Hand with triplet: 1m 1m 1m 2m 3m 4m 5m 6m 7m 8m 9m 1p 2p
    Hand hand1({0, 1, 2, 4, 8, 12, 16, 20, 24, 28, 32, 36, 40}, Wind::East, Wind::East);

    // Can kan 1m (have 3 in hand)
    TEST_ASSERT(hand1.canKan(3), "canKan 1m when have 3 in hand");

    // Cannot kan 2m (only have 1)
    TEST_ASSERT(!hand1.canKan(4), "cannot canKan 2m when only have 1");

    return 0;
}

// Test canAnkan functionality
int testCanAnkan() {
    std::cout << "\n=== Testing canAnkan ===" << std::endl;

    // Hand with triplet: 1m 1m 1m 2m 3m 4m 5m 6m 7m 8m 9m 1p 2p
    Hand hand1({0, 1, 2, 4, 8, 12, 16, 20, 24, 28, 32, 36, 40}, Wind::East, Wind::East);

    // Can ankan 1m (have 3 in hand, need 4th from draw)
    TEST_ASSERT(hand1.canAnkan(3), "canAnkan 1m when have 3 in hand");

    return 0;
}

int main() {
    int failed = 0;

    failed += testCanChi();
    failed += testCanPon();
    failed += testCanKan();
    failed += testCanAnkan();

    std::cout << "\n=== Test Summary ===" << std::endl;
    if (failed == 0) {
        std::cout << "All hand action tests passed!" << std::endl;
    } else {
        std::cout << failed << " test(s) failed!" << std::endl;
    }

    return failed;
}
