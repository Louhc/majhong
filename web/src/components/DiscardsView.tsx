import React from 'react';
import { Tile } from './Tile';

interface DiscardsViewProps {
  discards: number[];
  position: 'bottom' | 'right' | 'top' | 'left';
  playerName?: string;
  isCurrentPlayer?: boolean;
  score?: number;
}

export const DiscardsView: React.FC<DiscardsViewProps> = ({
  discards,
  position,
  playerName = '',
  isCurrentPlayer = false,
  score = 25000,
}) => {
  // 每行6张牌
  const rows: number[][] = [];
  for (let i = 0; i < discards.length; i += 6) {
    rows.push(discards.slice(i, i + 6));
  }

  const isVertical = position === 'left' || position === 'right';

  return (
    <div
      className={`
        flex flex-col items-center gap-2
        ${isVertical ? 'w-24' : 'w-auto'}
      `}
    >
      {/* 玩家信息 */}
      <div className={`
        text-center px-3 py-1 rounded
        ${isCurrentPlayer ? 'bg-yellow-500 text-black' : 'bg-gray-700 text-white'}
      `}>
        <div className="text-sm font-bold">{playerName}</div>
        <div className="text-xs">{score.toLocaleString()} 点</div>
      </div>

      {/* 牌河 */}
      <div className={`
        flex flex-col gap-0.5
        ${position === 'top' ? 'rotate-180' : ''}
        ${position === 'left' ? '-rotate-90' : ''}
        ${position === 'right' ? 'rotate-90' : ''}
      `}>
        {rows.map((row, rowIndex) => (
          <div key={rowIndex} className="flex gap-0.5">
            {row.map((tile, tileIndex) => (
              <Tile
                key={`${rowIndex}-${tileIndex}`}
                tileId={tile}
                small
              />
            ))}
          </div>
        ))}
      </div>
    </div>
  );
};
