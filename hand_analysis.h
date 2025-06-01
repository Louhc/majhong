#ifndef HAND_ANALYSIS_H
#define HAND_ANALYSIS_H

#include <vector>
#include <string>

typedef char Tile;
typedef std::string TileName;
typedef std::vector<char> TileCounts;

Tile yao[] = {1, 9, 10, 18, 19, 27, 28, 29, 30, 31, 32, 33, 34}; // Terminal and honor tiles

bool isValidTile(Tile tile) {
    return tile >= 1 && tile <= 34; // Valid tiles are from 1 to 34
}
bool isValidTileName(const TileName& tileName) {
    if (tileName.length() != 2) return false;
    char suit = tileName.back();
    if (suit != 'm' && suit != 'p' && suit != 's' && suit != 'z') return false;
    
    try {
        int rank = std::stoi(tileName.substr(0, tileName.length() - 1));
        if ( suit == 'z' ) {
            return rank >= 1 && rank <= 7; // Honor tiles (1z-7z)
        } else{
            return rank >= 1 && rank <= 9;
        }
    } catch (...) {
        return false; // Invalid rank
    }
}
TileName tile2name(Tile tile){
    if ( !isValidTile(tile) ) {
        return "Invalid Tile";
    }
    const char* suits[] = {"m", "p", "s", "z"};
    Tile suitIndex = (tile - 1) / 9;
    Tile rank = (tile - 1) % 9 + 1;
    return std::to_string((int)rank) + suits[suitIndex];
}
Tile name2tile(const TileName& tileName) {
    if (tileName.length() != 2) {
        return 0; // Invalid tile name
    }
    Tile rank = std::stoi(tileName.substr(0, tileName.length() - 1));
    char suit = tileName.back();
    
    if (rank < 1 || rank > 9) {
        return 0; // Invalid rank
    }
    
    Tile suitIndex;
    if (suit == 'm') suitIndex = 0;
    else if (suit == 'p') suitIndex = 1;
    else if (suit == 's') suitIndex = 2;
    else if (suit == 'z') suitIndex = 3;
    else return 0; // Invalid suit
    
    return (suitIndex * 9) + rank;
}
std::vector<Tile> getTileCounts(const std::vector<Tile> &hand, Tile drawnTile = 0) {
    std::vector<Tile> counts(34, 0);
    for (Tile tile : hand) {
        if (!isValidTile(tile)) continue; // Ignore invalid tiles
        counts[tile - 1]++;
    }
    if (isValidTile(drawnTile)) {
        counts[drawnTile - 1]++;
    }
    return counts;
}

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
bool isKokushiMuso(const std::vector<Tile> &hand, Tile drawnTile) {
    // Kokushi Musou (Thirteen Orphans) requires 13 unique terminal or honor tiles
    if (hand.size() != 13) return false;
    
    TileCounts counts = getTileCounts(hand, drawnTile);
    
    // Check for the required tiles: 1m, 9m, 1p, 9p, 1s, 9s, and all honors
    const int requiredTiles[] = {1, 9, 10, 18, 19, 27, 28, 29, 30, 31, 32, 33, 34};
    for (int tile : requiredTiles) {
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