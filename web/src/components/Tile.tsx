import React from 'react';

interface TileProps {
  tileId: number;
  onClick?: () => void;
  selected?: boolean;
  faceDown?: boolean;
  small?: boolean;
  horizontal?: boolean;
}

// 牌面字符 (使用 Unicode 麻将字符)
const TILE_CHARS: Record<number, string> = {
  // 万子 0-8
  0: '🀇', 1: '🀈', 2: '🀉', 3: '🀊', 4: '🀋', 5: '🀌', 6: '🀍', 7: '🀎', 8: '🀏',
  // 筒子 9-17
  9: '🀙', 10: '🀚', 11: '🀛', 12: '🀜', 13: '🀝', 14: '🀞', 15: '🀟', 16: '🀠', 17: '🀡',
  // 索子 18-26
  18: '🀐', 19: '🀑', 20: '🀒', 21: '🀓', 22: '🀔', 23: '🀕', 24: '🀖', 25: '🀗', 26: '🀘',
  // 字牌 27-33: 东南西北白发中
  27: '🀀', 28: '🀁', 29: '🀂', 30: '🀃', 31: '🀆', 32: '🀅', 33: '🀄',
};

// 牌名
const TILE_NAMES: Record<number, string> = {
  0: '一万', 1: '二万', 2: '三万', 3: '四万', 4: '五万', 5: '六万', 6: '七万', 7: '八万', 8: '九万',
  9: '一筒', 10: '二筒', 11: '三筒', 12: '四筒', 13: '五筒', 14: '六筒', 15: '七筒', 16: '八筒', 17: '九筒',
  18: '一索', 19: '二索', 20: '三索', 21: '四索', 22: '五索', 23: '六索', 24: '七索', 25: '八索', 26: '九索',
  27: '东', 28: '南', 29: '西', 30: '北', 31: '白', 32: '发', 33: '中',
};

export const Tile: React.FC<TileProps> = ({
  tileId,
  onClick,
  selected = false,
  faceDown = false,
  small = false,
  horizontal = false,
}) => {
  const tileIndex = Math.floor(tileId / 4);
  const char = faceDown ? '🀫' : (TILE_CHARS[tileIndex] || '?');
  const name = faceDown ? '牌背' : (TILE_NAMES[tileIndex] || '未知');

  const baseSize = small ? 'text-2xl' : 'text-4xl';
  const dimensions = small
    ? (horizontal ? 'w-8 h-6' : 'w-6 h-8')
    : (horizontal ? 'w-12 h-9' : 'w-9 h-12');

  return (
    <button
      className={`
        ${dimensions}
        ${baseSize}
        flex items-center justify-center
        bg-gradient-to-b from-white to-gray-100
        border border-gray-300 rounded
        shadow-md hover:shadow-lg
        transition-all duration-150
        ${selected ? 'ring-2 ring-yellow-400 -translate-y-2' : ''}
        ${onClick ? 'cursor-pointer hover:-translate-y-1' : 'cursor-default'}
        ${horizontal ? 'rotate-90' : ''}
      `}
      onClick={onClick}
      title={name}
      disabled={!onClick}
    >
      {char}
    </button>
  );
};

// 牌背组件
export const TileBack: React.FC<{ count?: number; small?: boolean }> = ({
  count = 1,
  small = false,
}) => {
  return (
    <div className="flex gap-0.5">
      {Array.from({ length: Math.min(count, 14) }).map((_, i) => (
        <Tile key={i} tileId={0} faceDown small={small} />
      ))}
      {count > 14 && <span className="text-gray-500 text-sm ml-1">+{count - 14}</span>}
    </div>
  );
};

// 获取牌名
export function getTileName(tileId: number): string {
  const tileIndex = Math.floor(tileId / 4);
  return TILE_NAMES[tileIndex] || '未知';
}

// 按牌型排序
export function sortHand(hand: number[]): number[] {
  return [...hand].sort((a, b) => Math.floor(a / 4) - Math.floor(b / 4));
}
