import React from 'react';
import { TableView } from './TableView';
import { HandView } from './HandView';
import { ActionBar } from './ActionBar';
import { GameState } from '../types';

interface GamePageProps {
  gameState: GameState | null;
  hand: number[];
  seat: number;
  availableActions: {
    canDiscard: boolean;
    canTsumo: boolean;
    canRon: boolean;
    canRiichi: boolean;
    canChi: boolean;
    canPon: boolean;
    canKan: boolean;
  };
  onDiscard: (tile: number) => void;
  onTsumo: () => void;
  onRon: () => void;
  onRiichi: (tile: number) => void;
  onChi: () => void;
  onPon: () => void;
  onKan: () => void;
  onPass: () => void;
  messages: string[];
}

export const GamePage: React.FC<GamePageProps> = ({
  gameState,
  hand,
  seat,
  availableActions,
  onDiscard,
  onTsumo,
  onRon,
  onRiichi,
  onChi,
  onPon,
  onKan,
  onPass,
  messages,
}) => {
  const handleRiichi = () => {
    // 简单处理: 立直时打出最后一张牌
    if (hand.length > 0) {
      onRiichi(hand[hand.length - 1]);
    }
  };

  return (
    <div className="min-h-screen bg-gray-900 flex flex-col">
      {/* 牌桌区域 */}
      <div className="flex-1 p-4">
        <TableView gameState={gameState} mySeat={seat} />
      </div>

      {/* 动作按钮 */}
      <div className="flex justify-center">
        <ActionBar
          canTsumo={availableActions.canTsumo}
          canRon={availableActions.canRon}
          canRiichi={availableActions.canRiichi}
          canChi={availableActions.canChi}
          canPon={availableActions.canPon}
          canKan={availableActions.canKan}
          onTsumo={onTsumo}
          onRon={onRon}
          onRiichi={handleRiichi}
          onChi={onChi}
          onPon={onPon}
          onKan={onKan}
          onPass={onPass}
        />
      </div>

      {/* 手牌区域 */}
      <div className="p-4 bg-gray-800">
        <HandView
          hand={hand}
          canDiscard={availableActions.canDiscard}
          onDiscard={onDiscard}
          lastDraw={gameState?.lastDraw}
        />
      </div>

      {/* 消息日志 */}
      <div className="h-24 bg-gray-900 border-t border-gray-700 overflow-y-auto p-2">
        {messages.slice(-5).map((msg, i) => (
          <div key={i} className="text-gray-400 text-sm">
            {msg}
          </div>
        ))}
      </div>
    </div>
  );
};
