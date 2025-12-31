import React from 'react';
import { TileBack } from './Tile';
import { DiscardsView } from './DiscardsView';
import { GameState, Wind } from '../types';

interface TableViewProps {
  gameState: GameState | null;
  mySeat: number;
}

const WIND_NAMES: Record<Wind, string> = {
  east: '东',
  south: '南',
  west: '西',
  north: '北',
};

const SEAT_NAMES = ['东', '南', '西', '北'];

export const TableView: React.FC<TableViewProps> = ({
  gameState,
  mySeat,
}) => {
  if (!gameState) {
    return (
      <div className="flex items-center justify-center h-96 text-white">
        等待游戏开始...
      </div>
    );
  }

  // 计算相对座位 (自己在下方)
  const getRelativeSeat = (offset: number) => (mySeat + offset) % 4;
  const bottomSeat = mySeat;
  const rightSeat = getRelativeSeat(1);
  const topSeat = getRelativeSeat(2);
  const leftSeat = getRelativeSeat(3);

  return (
    <div className="relative w-full h-[600px] bg-mahjong-green rounded-xl overflow-hidden">
      {/* 中央信息 */}
      <div className="absolute top-1/2 left-1/2 -translate-x-1/2 -translate-y-1/2
                      w-32 h-32 bg-gray-800 rounded-lg flex flex-col items-center justify-center
                      text-white shadow-lg">
        <div className="text-lg font-bold">
          {WIND_NAMES[gameState.roundWind]}场
        </div>
        <div className="text-sm">
          {gameState.honba > 0 && `${gameState.honba}本场`}
        </div>
        <div className="text-2xl font-bold text-yellow-400">
          {gameState.remainingTiles}
        </div>
        <div className="text-xs text-gray-400">剩余牌数</div>
      </div>

      {/* 上家 (对面) */}
      <div className="absolute top-4 left-1/2 -translate-x-1/2">
        <DiscardsView
          discards={gameState.discards[topSeat] || []}
          position="top"
          playerName={`${SEAT_NAMES[topSeat]}家`}
          isCurrentPlayer={gameState.currentPlayer === topSeat}
          score={gameState.scores[topSeat]}
        />
      </div>

      {/* 右家 */}
      <div className="absolute right-4 top-1/2 -translate-y-1/2">
        <DiscardsView
          discards={gameState.discards[rightSeat] || []}
          position="right"
          playerName={`${SEAT_NAMES[rightSeat]}家`}
          isCurrentPlayer={gameState.currentPlayer === rightSeat}
          score={gameState.scores[rightSeat]}
        />
      </div>

      {/* 下家 (自己) */}
      <div className="absolute bottom-4 left-1/2 -translate-x-1/2">
        <DiscardsView
          discards={gameState.discards[bottomSeat] || []}
          position="bottom"
          playerName={`${SEAT_NAMES[bottomSeat]}家 (你)`}
          isCurrentPlayer={gameState.currentPlayer === bottomSeat}
          score={gameState.scores[bottomSeat]}
        />
      </div>

      {/* 左家 */}
      <div className="absolute left-4 top-1/2 -translate-y-1/2">
        <DiscardsView
          discards={gameState.discards[leftSeat] || []}
          position="left"
          playerName={`${SEAT_NAMES[leftSeat]}家`}
          isCurrentPlayer={gameState.currentPlayer === leftSeat}
          score={gameState.scores[leftSeat]}
        />
      </div>

      {/* 其他玩家手牌 (牌背) */}
      <div className="absolute top-16 left-1/2 -translate-x-1/2">
        <TileBack count={13} small />
      </div>
      <div className="absolute right-16 top-1/2 -translate-y-1/2 rotate-90">
        <TileBack count={13} small />
      </div>
      <div className="absolute left-16 top-1/2 -translate-y-1/2 -rotate-90">
        <TileBack count={13} small />
      </div>
    </div>
  );
};
