#include "printer.h"

void printTileIndex(const TileIndex& tile_index) {
    std::cout << getTileName(tile_index) << " ";
}

void printTile(const Tile& tile) {
    std::cout << getTileName(tile * 4 + 1) << " ";
}

void printHand(const TileIndexList& hand) {
    for (const TileIndex& tile_index : hand) {
        printTileIndex(tile_index);
    }
}

void printMeldType(const MeldType& meld_type) {
    std::cout << "<";
    switch (meld_type) {
        case MeldType::Ankan:          std::cout << "Ankan"; break;
        case MeldType::Minkan:         std::cout << "Minkan"; break;
        case MeldType::Chakan:         std::cout << "Chakan"; break;
        case MeldType::Chi:            std::cout << "Chi"; break;
        case MeldType::Pon:            std::cout << "Pon"; break;
        case MeldType::ClosedSequence: std::cout << "Closed Sequence"; break;
        case MeldType::ClosedTriplet:  std::cout << "Closed Triplet"; break;
        case MeldType::Pair:           std::cout << "Pair"; break;
        default:                       std::cout << "Unknown";
    }
    std::cout << ">";
}

void printMeld(const TileMeld& meld) {
    std::cout << "<";
    switch (meld.type) {
        case MeldType::Ankan: case MeldType::Minkan: case MeldType::Chakan:
            std::cout << getTileName(meld.tile * 4) << " "
                      << getTileName(meld.tile * 4 + 1) << " "
                      << getTileName(meld.tile * 4 + 2) << " "
                      << getTileName(meld.tile * 4 + 3);
            break;
        case MeldType::Chi: case MeldType::ClosedSequence:
            std::cout << getTileName(meld.tile * 4 + 1) << " "
                      << getTileName(meld.tile * 4 + 5) << " "
                      << getTileName(meld.tile * 4 + 9);
            break;
        case MeldType::ClosedTriplet: case MeldType::Pon:
            std::cout << getTileName(meld.tile * 4 + 1) << " "
                      << getTileName(meld.tile * 4 + 2) << " "
                      << getTileName(meld.tile * 4 + 3);
            break;
        case MeldType::Pair:
            std::cout << getTileName(meld.tile * 4 + 1) << " "
                      << getTileName(meld.tile * 4 + 2);
            break;
        default:
            std::cout << "Unknown";
    }
    std::cout << ">";
}

void printTileMeldList(const TileMeldList& meld_list) {
    for (const TileMeld& meld : meld_list) {
        printMeld(meld); std::cout << " ";
    } std::cout << std::endl;
}

void printHandParseResult(const HandParseResult& result){
    int id = 0;
    for (const auto& melds : result) {
        std::cout << "Parse Result " << id << " :" << std::endl;
        ++id;
        printTileMeldList(melds);
    } std::cout << std::endl;
}
