// 牌的类型定义
export type TileType = 'man' | 'pin' | 'sou' | 'honor';

export interface Tile {
  id: number;       // 0-135 唯一标识
  type: TileType;   // 牌类型
  value: number;    // 1-9 for man/pin/sou, 1-7 for honor
  name: string;     // 显示名称
}

// 风位
export type Wind = 'east' | 'south' | 'west' | 'north';

// 动作类型
export enum Action {
  Pass = -1,
  Win = -2,
  Riichi = -3,
  Chi = -4,
  Pon = -5,
  Kan = -6,
  AnKan = -7,
}

// 消息类型
export enum MessageType {
  CreateRoom = 'create_room',
  JoinRoom = 'join_room',
  LeaveRoom = 'leave_room',
  Ready = 'ready',
  Action = 'action',
  RoomCreated = 'room_created',
  RoomJoined = 'room_joined',
  PlayerJoined = 'player_joined',
  PlayerLeft = 'player_left',
  GameStart = 'game_start',
  YourTurn = 'your_turn',
  PlayerAction = 'player_action',
  GameState = 'game_state',
  GameEnd = 'game_end',
  Error = 'error',
}

// 游戏状态
export interface GameState {
  roundWind: Wind;
  dealer: number;
  currentPlayer: number;
  remainingTiles: number;
  honba: number;
  riichiSticks: number;
  scores: number[];
  riichiStatus: boolean[];
  hands: number[][];      // 手牌 (只有自己的可见)
  discards: number[][];   // 各家牌河
  melds: number[][][];    // 各家副露
  canTsumo: boolean;
  canRon: boolean;
  canRiichi: boolean;
  canChi: boolean;
  canPon: boolean;
  canKan: boolean;
  lastDraw: number;
  lastDiscard: number;
  lastDiscardPlayer: number;
}

// 服务器消息
export interface ServerMessage {
  type: MessageType;
  roomId?: string;
  seat?: number;
  tile?: number;
  action?: number;
  state?: GameState;
  hand?: number[];
  options?: string[];
  winner?: number;
  yaku?: string[];
  score?: number;
  errorMsg?: string;
}

// 客户端消息
export interface ClientMessage {
  type: MessageType;
  roomId?: string;
  action?: number;
  tile?: number;
}

// 房间信息
export interface RoomInfo {
  roomId: string;
  playerCount: number;
  state: 'waiting' | 'playing' | 'finished';
}
