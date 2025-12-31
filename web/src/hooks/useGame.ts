import { useState, useCallback, useMemo } from 'react';
import { GameState, ServerMessage, MessageType, Action } from '../types';
import { useWebSocket } from './useWebSocket';

type GamePhase = 'lobby' | 'waiting' | 'playing' | 'finished';

interface UseGameReturn {
  // 连接状态
  connected: boolean;

  // 游戏阶段
  phase: GamePhase;

  // 房间信息
  roomId: string | null;
  seat: number;

  // 游戏状态
  gameState: GameState | null;
  hand: number[];

  // 可执行的动作
  availableActions: {
    canDiscard: boolean;
    canTsumo: boolean;
    canRon: boolean;
    canRiichi: boolean;
    canChi: boolean;
    canPon: boolean;
    canKan: boolean;
  };

  // 动作方法
  createRoom: () => void;
  joinRoom: (roomId: string) => void;
  leaveRoom: () => void;
  ready: () => void;
  discard: (tile: number) => void;
  declareWin: () => void;
  declareRiichi: (tile: number) => void;
  declareChi: () => void;
  declarePon: () => void;
  declareKan: () => void;
  pass: () => void;

  // 消息记录
  messages: string[];
}

export function useGame(serverUrl: string): UseGameReturn {
  const [phase, setPhase] = useState<GamePhase>('lobby');
  const [roomId, setRoomId] = useState<string | null>(null);
  const [seat, setSeat] = useState<number>(-1);
  const [gameState, setGameState] = useState<GameState | null>(null);
  const [hand, setHand] = useState<number[]>([]);
  const [messages, setMessages] = useState<string[]>([]);

  const addMessage = useCallback((msg: string) => {
    setMessages(prev => [...prev.slice(-50), msg]);
  }, []);

  const handleMessage = useCallback((message: ServerMessage) => {
    switch (message.type) {
      case MessageType.RoomCreated:
        setRoomId(message.roomId || null);
        setSeat(message.seat || 0);
        setPhase('waiting');
        addMessage(`房间 ${message.roomId} 已创建，你的座位是 ${getSeatName(message.seat || 0)}`);
        break;

      case MessageType.RoomJoined:
        setRoomId(message.roomId || null);
        setSeat(message.seat || 0);
        setPhase('waiting');
        addMessage(`已加入房间 ${message.roomId}，你的座位是 ${getSeatName(message.seat || 0)}`);
        break;

      case MessageType.PlayerJoined:
        addMessage(`${getSeatName(message.seat || 0)} 加入了房间`);
        break;

      case MessageType.PlayerLeft:
        addMessage(`${getSeatName(message.seat || 0)} 离开了房间`);
        break;

      case MessageType.GameStart:
        setPhase('playing');
        if (message.hand) {
          setHand(message.hand);
        }
        addMessage('游戏开始！');
        break;

      case MessageType.YourTurn:
        if (message.tile !== undefined && message.tile >= 0) {
          setHand(prev => [...prev, message.tile!]);
          addMessage(`你摸到了 ${getTileName(message.tile)}`);
        }
        break;

      case MessageType.PlayerAction:
        addMessage(`${getSeatName(message.seat || 0)} ${getActionName(message.action || 0, message.tile)}`);
        break;

      case MessageType.GameState:
        if (message.state) {
          setGameState(message.state);
        }
        break;

      case MessageType.GameEnd:
        setPhase('finished');
        addMessage(`游戏结束！${getSeatName(message.winner || 0)} 获胜`);
        if (message.yaku) {
          addMessage(`役: ${message.yaku.join(', ')}`);
        }
        if (message.score) {
          addMessage(`得分: ${message.score}`);
        }
        break;

      case MessageType.Error:
        addMessage(`错误: ${message.errorMsg}`);
        break;
    }
  }, [addMessage]);

  const { connected, createRoom, joinRoom: wsJoinRoom, leaveRoom: wsLeaveRoom, ready, sendAction } = useWebSocket({
    url: serverUrl,
    onMessage: handleMessage,
    onConnect: () => addMessage('已连接到服务器'),
    onDisconnect: () => addMessage('与服务器断开连接'),
  });

  const joinRoom = useCallback((id: string) => {
    wsJoinRoom(id);
  }, [wsJoinRoom]);

  const leaveRoom = useCallback(() => {
    wsLeaveRoom();
    setPhase('lobby');
    setRoomId(null);
    setSeat(-1);
    setGameState(null);
    setHand([]);
  }, [wsLeaveRoom]);

  const discard = useCallback((tile: number) => {
    sendAction(tile);
    setHand(prev => {
      const idx = prev.indexOf(tile);
      if (idx >= 0) {
        const newHand = [...prev];
        newHand.splice(idx, 1);
        return newHand;
      }
      return prev;
    });
  }, [sendAction]);

  const declareWin = useCallback(() => {
    sendAction(Action.Win);
  }, [sendAction]);

  const declareRiichi = useCallback((tile: number) => {
    sendAction(Action.Riichi, tile);
    setHand(prev => {
      const idx = prev.indexOf(tile);
      if (idx >= 0) {
        const newHand = [...prev];
        newHand.splice(idx, 1);
        return newHand;
      }
      return prev;
    });
  }, [sendAction]);

  const declareChi = useCallback(() => {
    sendAction(Action.Chi);
  }, [sendAction]);

  const declarePon = useCallback(() => {
    sendAction(Action.Pon);
  }, [sendAction]);

  const declareKan = useCallback(() => {
    sendAction(Action.Kan);
  }, [sendAction]);

  const pass = useCallback(() => {
    sendAction(Action.Pass);
  }, [sendAction]);

  const availableActions = useMemo(() => ({
    canDiscard: phase === 'playing' && gameState?.currentPlayer === seat,
    canTsumo: gameState?.canTsumo || false,
    canRon: gameState?.canRon || false,
    canRiichi: gameState?.canRiichi || false,
    canChi: gameState?.canChi || false,
    canPon: gameState?.canPon || false,
    canKan: gameState?.canKan || false,
  }), [phase, gameState, seat]);

  return {
    connected,
    phase,
    roomId,
    seat,
    gameState,
    hand,
    availableActions,
    createRoom,
    joinRoom,
    leaveRoom,
    ready,
    discard,
    declareWin,
    declareRiichi,
    declareChi,
    declarePon,
    declareKan,
    pass,
    messages,
  };
}

// 辅助函数
function getSeatName(seat: number): string {
  const names = ['东家', '南家', '西家', '北家'];
  return names[seat] || `座位${seat}`;
}

function getTileName(tileId: number): string {
  const tile = Math.floor(tileId / 4);
  if (tile < 9) return `${tile + 1}万`;
  if (tile < 18) return `${tile - 8}筒`;
  if (tile < 27) return `${tile - 17}条`;
  const honors = ['东', '南', '西', '北', '白', '发', '中'];
  return honors[tile - 27] || '?';
}

function getActionName(action: number, tile?: number): string {
  if (action >= 0) {
    return `打出 ${getTileName(action)}`;
  }
  switch (action) {
    case Action.Win: return '和牌！';
    case Action.Riichi: return `立直！打出 ${tile !== undefined ? getTileName(tile) : ''}`;
    case Action.Chi: return '吃！';
    case Action.Pon: return '碰！';
    case Action.Kan: return '杠！';
    case Action.AnKan: return '暗杠！';
    case Action.Pass: return '过';
    default: return '未知动作';
  }
}
