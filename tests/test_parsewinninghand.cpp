#include <iostream>
#include "types.h"
#include "printer.h"

int main(){
    std::cout << "Test 1:" << std::endl;
    Hand hand1({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12}, Wind::East, Wind::East);
    auto res1 = hand1.parseWinningHand(Tile(13));
    printHandParseResult(res1);
    
    
    std::cout << "Test 2:" << std::endl;
    Hand hand2({0, 1, 2, 4, 8, 12, 16, 20, 24, 28, 32, 33, 34}, Wind::East, Wind::East);
    auto res2 = hand2.parseWinningHand(Tile(3));
    printHandParseResult(res2);

    std::cout << "Test 3:" << std::endl;
    Hand hand3({0, 1, 2, 4, 8, 12, 16, 20, 24, 28, 32, 33, 34}, Wind::East, Wind::East);
    auto res3 = hand3.parseWinningHand(Tile(50));
    printHandParseResult(res3);
    return 0;
}