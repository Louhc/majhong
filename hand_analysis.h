#ifndef HAND_ANALYSIS_H
#define HAND_ANALYSIS_H

#include <vector>
#include <string>
#include "types.h"

bool isRyanpeikou(const std::vector<Tile> &hand, Tile drawnTile) {
    // Ryanpeikou (Double Sequence) requires two sets of identical sequences
    if (hand.size() != 13) return false;

    TileCounts counts = getTileCounts(hand, drawnTile);
    
    int sequenceCount = 0, pairCount = 0;
    for ( int i = 0; i < 27; ++i ){
        if ( i % 9 >= 7 ) continue;
        if ( counts[i] >= 2 && counts[i + 1] >= 2 && counts[i + 2] >= 2 ) {
            sequenceCount++; counts[i] -= 2; counts[i + 1] -= 2;
            counts[i + 2] -= 2; --i;
        }
    }
    for ( int i = 0; i < 34; ++i ){
        if ( counts[i] >= 2 ) {
            pairCount++;
        }
    }

    return sequenceCount == 2 && pairCount == 1;
}
bool isKokushiMusoJusanmen(const std::vector<Tile> &hand, Tile drawnTile) {
    // Kokushi Musou (Thirteen Orphans) requires 13 unique terminal or honor tiles
    if (hand.size() != 13) return false;
    
    TileCounts counts = getTileCounts(hand);
    bool hasDrawnTile = false;
    
    // Check for the required tiles: 1m, 9m, 1p, 9p, 1s, 9s, and all honors
    const Tile requiredTiles[] = {1, 9, 10, 18, 19, 27, 28, 29, 30, 31, 32, 33, 34};
    for (Tile tile : requiredTiles) {
        if (counts[tile - 1] == 0) return false;
        if ( tile == drawnTile ) {
            hasDrawnTile = true; // Drawn tile must be one of the required tiles
        }
    }
    if (!hasDrawnTile) return false; // Drawn tile must be one of the required tiles
    
    return true;
}
bool isKokushiMuso(const std::vector<Tile> &hand, Tile drawnTile) {
    // Kokushi Musou (Thirteen Orphans) requires 13 unique terminal or honor tiles
    if (hand.size() != 13) return false;
    // if (isKokushiMusoJusanmen(hand, drawnTile)) return false; // Jusanmen variant
    
    TileCounts counts = getTileCounts(hand, drawnTile);
    
    // Check for the required tiles: 1m, 9m, 1p, 9p, 1s, 9s, and all honors
    const Tile requiredTiles[] = {1, 9, 10, 18, 19, 27, 28, 29, 30, 31, 32, 33, 34};
    for (Tile tile : requiredTiles) {
        if (counts[tile - 1] == 0) return false;
    }
    
    return true;
}
bool isChiitoitsu(const std::vector<Tile> &hand, Tile drawnTile) {
    // Chiitoitsu (Seven Pairs) requires 7 pairs and can include the drawn tile
    if (hand.size() != 13) return false;
    if (isRyanpeikou(hand, drawnTile)) return false; // Cannot be both Ryanpeikou and Chiitoitsu

    TileCounts counts = getTileCounts(hand, drawnTile);

    int pairs = 0;
    for (char count : counts) {
        if (count == 2) pairs++;
        else if (count != 0) return false; // Invalid hand
    }
    
    return pairs == 7;
}
bool isTanyao(const std::vector<Tile> &hand, Tile drawnTile) {
    // Tanyao (All Simples) requires no terminal or honor tiles
    TileCounts counts = getTileCounts(hand, drawnTile);
    
    for ( int i = 0; i < 13; ++i ){
        if ( counts[yao[i]] > 0) return false; // Contains terminal or honor tiles
    }
    
    return true;
}

bool isWinningHand(const std::vector<Tile> &hand, Tile drawnTile) {
    if ( hand.size() != 13 ) return false; // Winning hand must have 13 tiles
    if ( isKokushiMuso(hand, drawnTile) || isChiitoitsu(hand, drawnTile) ) return true; // Kokushi Musou or Chiitoitsu
    return false;
}



#endif // HAND_ANALYSIS_H