import React, { useState } from 'react';
import { Tile, sortHand } from './Tile';

interface HandViewProps {
  hand: number[];
  canDiscard: boolean;
  onDiscard: (tile: number) => void;
  lastDraw?: number;
}

export const HandView: React.FC<HandViewProps> = ({
  hand,
  canDiscard,
  onDiscard,
  lastDraw,
}) => {
  const [selectedTile, setSelectedTile] = useState<number | null>(null);

  // 排序手牌，但保持最后摸的牌在右侧
  const sortedHand = React.useMemo(() => {
    if (lastDraw === undefined) {
      return sortHand(hand);
    }
    const withoutDraw = hand.filter(t => t !== lastDraw);
    return [...sortHand(withoutDraw), lastDraw];
  }, [hand, lastDraw]);

  const handleTileClick = (tile: number) => {
    if (!canDiscard) return;

    if (selectedTile === tile) {
      // 双击打出
      onDiscard(tile);
      setSelectedTile(null);
    } else {
      setSelectedTile(tile);
    }
  };

  const handleDiscardClick = () => {
    if (selectedTile !== null) {
      onDiscard(selectedTile);
      setSelectedTile(null);
    }
  };

  return (
    <div className="flex flex-col items-center gap-4">
      {/* 手牌区 */}
      <div className="flex gap-1 p-4 bg-mahjong-felt rounded-lg shadow-inner">
        {sortedHand.map((tile, index) => {
          const isDraw = tile === lastDraw && index === sortedHand.length - 1;
          return (
            <div key={`${tile}-${index}`} className={isDraw ? 'ml-4' : ''}>
              <Tile
                tileId={tile}
                selected={selectedTile === tile}
                onClick={canDiscard ? () => handleTileClick(tile) : undefined}
              />
            </div>
          );
        })}
      </div>

      {/* 操作提示 */}
      {canDiscard && (
        <div className="flex items-center gap-4">
          <span className="text-white text-sm">
            {selectedTile !== null ? '再次点击打出，或选择其他牌' : '点击选择要打出的牌'}
          </span>
          {selectedTile !== null && (
            <button
              className="px-4 py-2 bg-red-500 text-white rounded hover:bg-red-600 transition-colors"
              onClick={handleDiscardClick}
            >
              打出
            </button>
          )}
        </div>
      )}
    </div>
  );
};
