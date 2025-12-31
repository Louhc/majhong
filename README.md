# Mahjong Game (日本麻将)

一个用 C++ 实现的日本麻将（立直麻将）游戏，支持 AI 对战和多人联机。

## 功能特性

- **完整的日本麻将规则**
  - 和牌判定（标准型、七对子、国士无双）
  - 向听数计算
  - 完整的役种判定（含役满）
  - 符数和得点计算

- **AI 对战**
  - 简单 AI（基于牌效率的弃牌选择）
  - 支持副露判断（碰、杠）

- **多人联机**（框架已实现）
  - WebSocket 服务器
  - 房间管理系统
  - JSON 消息协议

- **Web 前端**
  - React + TypeScript
  - 实时游戏界面
  - 响应式设计

## 项目结构

```
majhong/
├── src/
│   ├── hand/                 # 手牌核心逻辑
│   │   ├── types.h           # 类型定义 (Tile, Yaku, Meld 等)
│   │   ├── constants.cpp/h   # 牌组常量 (幺九牌、绿一色等)
│   │   ├── tiles.cpp/h       # 牌名映射
│   │   ├── hand_action.cpp   # 手牌操作 (吃、碰、杠)
│   │   ├── yaku_analysis.cpp # 役种判定
│   │   └── scoring.cpp/h     # 符数和得点计算
│   ├── game/                 # 游戏逻辑
│   │   ├── player.cpp/h      # 玩家基类
│   │   ├── simple_ai.cpp/h   # AI 实现
│   │   ├── table.cpp/h       # 牌桌和游戏流程
│   │   └── game_state.cpp/h  # 游戏状态序列化
│   ├── network/              # 网络模块
│   │   ├── session.cpp/h     # 玩家会话
│   │   ├── room.cpp/h        # 房间管理
│   │   └── server.cpp/h      # WebSocket 服务器
│   ├── display/              # 显示模块
│   │   └── printer.cpp/h     # 调试输出
│   └── main.cpp              # 程序入口
├── tests/                    # 测试文件
│   ├── test_yaku.cpp         # 役种测试
│   ├── test_hand_action.cpp  # 手牌操作测试
│   └── test_yakuman.cpp      # 役满测试
├── web/                      # Web 前端
│   ├── src/
│   │   ├── components/       # React 组件
│   │   ├── hooks/            # WebSocket Hook
│   │   └── types.ts          # TypeScript 类型
│   └── package.json
└── CMakeLists.txt
```

## 编译和运行

### 依赖

- C++17 兼容的编译器 (GCC 7+, Clang 5+, MSVC 2017+)
- CMake 3.10+
- Node.js 16+ (Web 前端)

### 编译后端

```bash
mkdir build && cd build
cmake ..
make
```

### 运行游戏

```bash
# 4个 AI 对战测试
./MahjongGame

# 运行测试
./test_yaku
./test_hand_action
./test_yakuman
```

### 运行 Web 前端

```bash
cd web
npm install
npm run dev      # 开发服务器 http://localhost:3000
npm run build    # 生产构建
```

## 役种支持

### 1 翻
- 立直、门清自摸、一发
- 断幺九、平和、一杯口
- 自风、场风、役牌（白发中）
- 抢杠、岭上开花、海底摸月、河底捞鱼

### 2 翻
- 两立直、七对子、对对和、三暗刻
- 三色同刻、三杠子、混老头、小三元

### 2-3 翻（副露降级）
- 混全带幺九、一气通贯、三色同顺
- 纯全带幺九、混一色

### 6 翻
- 清一色

### 役满
- 大三元、四暗刻、字一色、绿一色、清老头
- 国士无双、小四喜、四杠子、九莲宝灯

### 双倍役满
- 四暗刻单骑、国士无双十三面、纯正九莲宝灯、大四喜

## 得点计算

| 翻数 | 等级 | 闲家荣和 | 庄家荣和 |
|-----|------|---------|---------|
| 1翻30符 | - | 1000 | 1500 |
| 2翻40符 | - | 2600 | 3900 |
| 3翻30符 | - | 3900 | 5800 |
| 5翻 | 满贯 | 8000 | 12000 |
| 6-7翻 | 跳满 | 12000 | 18000 |
| 8-10翻 | 倍满 | 16000 | 24000 |
| 11-12翻 | 三倍满 | 24000 | 36000 |
| 13翻+ | 役满 | 32000 | 48000 |

## API 示例

### 役种判定

```cpp
#include "hand/hand.h"

// 创建手牌 (东场东家)
Hand hand({_1m, _2m, _3m, _4m, _5m, _6m, _7m, _8m, _9m, _1p, _1p, _1p, _2p},
          Wind::East, Wind::East);

// 检查是否能和牌
TileIndex draw = _2p * 4;  // 摸到 2筒
if (hand.isWinningHand(draw)) {
    // 计算役种
    YakuList yaku = hand.calcYaku(draw, true);  // true = 自摸

    // 显示役种
    std::cout << getYakuNames(yaku) << std::endl;
}
```

### 得点计算

```cpp
#include "hand/scoring.h"

// 3翻40符，闲家荣和
AgariResult result = calcScore(3, 40, false, false);
std::cout << "得点: " << result.total_points << std::endl;  // 5200
```

### 向听数计算

```cpp
Hand hand({...}, Wind::East, Wind::East);
int shanten = hand.calcShanten();
// -1 = 已和牌, 0 = 听牌, 1 = 一向听, ...
```

## 网络协议

### 消息类型

```typescript
enum MessageType {
  CreateRoom = 'create_room',
  JoinRoom = 'join_room',
  Ready = 'ready',
  Action = 'action',
  GameState = 'game_state',
  // ...
}
```

### 游戏状态

```json
{
  "roundWind": "east",
  "dealer": 0,
  "currentPlayer": 1,
  "remainingTiles": 70,
  "scores": [25000, 25000, 25000, 25000],
  "hands": [[...], [], [], []],
  "discards": [[...], [...], [...], [...]]
}
```

## 待完善

- [ ] 集成 WebSocket 库 (uWebSockets / libwebsockets)
- [ ] 完善网络同步逻辑
- [ ] 添加听牌提示
- [ ] 添加牌谱记录和回放
- [ ] 添加更智能的 AI

## License

MIT
