#ifndef PRINTER_H
#define PRINTER_H

#include <iostream> // 用于 std::ostream 和其他输出操作喵
#include <vector>   // 用于打印 TileList 等喵
#include <array>    // 用于打印 TileCounts 等喵
#include <string>   // 用于 TileName 等喵

// 包含你的麻将核心类型定义，以便输出函数能够访问它们喵
#include "types.h" // 包含 Tile, TileType, TileName, TileCounts, Wind 等喵
#include "tiles.h" // 包含 getTileName 等 Tile 相关函数喵

// 打印单个 Tile 的名称（例如 "1m", "5p", "东"）喵
void printTile(const Tile& tile);

// 打印单个 TileType 的名称喵
// (通常 TileType 需要转换为 TileName 才能有意义地打印)喵
void printTileType(const TileType& tile_type);

// 打印 TileList (即 std::vector<Tile>) 中的所有牌喵
void printTileList(const TileList& tile_list);

// 打印 TileCounts (即 std::array<int, 34>) 中的牌型计数喵
void printTileCounts(const TileCounts& tile_counts);

void printTileMeldList(const TileMeldList& meld_list);

// 打印 Hand 的详细信息（如手牌、副露、风等）喵
// 需要 Hand 类的定义是可见的喵
// void printHand(const Hand& hand); // 如果 Hand 类定义在别处，这里可能需要前向声明或包含对应头文件喵

// 打印 ParsedMeld (如果你已经定义了这个结构体)喵
// void printParsedMeld(const ParsedMeld& meld); // 如果 ParsedMeld 已定义喵

// 打印 HandParseResult (一个面子组合列表)喵
void printHandParseResult(const HandParseResult& result); // 如果 HandParseResult 已定义喵

// 你也可以为常用的麻将类型定义友元或成员的 operator<< 重载，
// 这样就可以直接使用 std::cout << your_object; 来打印了喵
// 例如喵：
// std::ostream& operator<<(std::ostream& os, const Tile& tile);
// std::ostream& operator<<(std::ostream& os, const TileType& tile_type);
// std::ostream& operator<<(std::ostream& os, const TileList& tile_list);
    // std::ostream& operator<<(std::ostream& os, const TileCounts& tile_counts);


#endif // PRINTER_H