import { useState, useEffect, useCallback, useRef } from 'react';
import { ServerMessage, ClientMessage, MessageType } from '../types';

interface UseWebSocketOptions {
  url: string;
  onMessage?: (message: ServerMessage) => void;
  onConnect?: () => void;
  onDisconnect?: () => void;
  onError?: (error: Event) => void;
}

interface UseWebSocketReturn {
  connected: boolean;
  send: (message: ClientMessage) => void;
  createRoom: () => void;
  joinRoom: (roomId: string) => void;
  leaveRoom: () => void;
  ready: () => void;
  sendAction: (action: number, tile?: number) => void;
}

export function useWebSocket({
  url,
  onMessage,
  onConnect,
  onDisconnect,
  onError,
}: UseWebSocketOptions): UseWebSocketReturn {
  const [connected, setConnected] = useState(false);
  const wsRef = useRef<WebSocket | null>(null);
  const reconnectTimeoutRef = useRef<number>();

  const connect = useCallback(() => {
    try {
      const ws = new WebSocket(url);

      ws.onopen = () => {
        setConnected(true);
        onConnect?.();
      };

      ws.onclose = () => {
        setConnected(false);
        onDisconnect?.();
        // 自动重连
        reconnectTimeoutRef.current = window.setTimeout(() => {
          connect();
        }, 3000);
      };

      ws.onerror = (error) => {
        onError?.(error);
      };

      ws.onmessage = (event) => {
        try {
          const message: ServerMessage = JSON.parse(event.data);
          onMessage?.(message);
        } catch (e) {
          console.error('Failed to parse message:', e);
        }
      };

      wsRef.current = ws;
    } catch (e) {
      console.error('WebSocket connection error:', e);
    }
  }, [url, onMessage, onConnect, onDisconnect, onError]);

  useEffect(() => {
    connect();

    return () => {
      if (reconnectTimeoutRef.current) {
        clearTimeout(reconnectTimeoutRef.current);
      }
      wsRef.current?.close();
    };
  }, [connect]);

  const send = useCallback((message: ClientMessage) => {
    if (wsRef.current?.readyState === WebSocket.OPEN) {
      wsRef.current.send(JSON.stringify(message));
    }
  }, []);

  const createRoom = useCallback(() => {
    send({ type: MessageType.CreateRoom });
  }, [send]);

  const joinRoom = useCallback((roomId: string) => {
    send({ type: MessageType.JoinRoom, roomId });
  }, [send]);

  const leaveRoom = useCallback(() => {
    send({ type: MessageType.LeaveRoom });
  }, [send]);

  const ready = useCallback(() => {
    send({ type: MessageType.Ready });
  }, [send]);

  const sendAction = useCallback((action: number, tile?: number) => {
    send({ type: MessageType.Action, action, tile });
  }, [send]);

  return {
    connected,
    send,
    createRoom,
    joinRoom,
    leaveRoom,
    ready,
    sendAction,
  };
}
