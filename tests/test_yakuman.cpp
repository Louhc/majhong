#include <iostream>
#include "types.h"
#include "printer.h"

int main(){
    std::cout << "Test 1:" << std::endl;
    Hand hand1({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12}, Wind::East, Wind::East);
    auto res1 = hand1.parseWinningHand(Tile(13));
    printHandParseResult(res1);
    return 0;
}