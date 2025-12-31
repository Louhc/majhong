import React from 'react';

interface WaitingRoomProps {
  roomId: string;
  seat: number;
  onReady: () => void;
  onLeave: () => void;
}

const SEAT_NAMES = ['东', '南', '西', '北'];

export const WaitingRoom: React.FC<WaitingRoomProps> = ({
  roomId,
  seat,
  onReady,
  onLeave,
}) => {
  const copyRoomId = () => {
    navigator.clipboard.writeText(roomId);
  };

  return (
    <div className="min-h-screen flex items-center justify-center bg-gradient-to-b from-gray-900 to-gray-800">
      <div className="bg-gray-800 p-8 rounded-xl shadow-2xl w-full max-w-md">
        <h2 className="text-2xl font-bold text-white text-center mb-6">
          等待玩家加入
        </h2>

        {/* 房间号 */}
        <div className="bg-gray-700 p-4 rounded-lg mb-6">
          <p className="text-gray-400 text-sm text-center mb-2">房间号</p>
          <div className="flex items-center justify-center gap-2">
            <span className="text-3xl font-mono font-bold text-yellow-400 tracking-wider">
              {roomId}
            </span>
            <button
              className="px-3 py-1 bg-gray-600 hover:bg-gray-500 text-white text-sm rounded
                         transition-colors"
              onClick={copyRoomId}
            >
              复制
            </button>
          </div>
        </div>

        {/* 座位信息 */}
        <div className="bg-gray-700 p-4 rounded-lg mb-6">
          <p className="text-gray-400 text-sm text-center mb-2">你的座位</p>
          <p className="text-2xl font-bold text-white text-center">
            {SEAT_NAMES[seat]}家
          </p>
        </div>

        {/* 座位列表 */}
        <div className="grid grid-cols-2 gap-3 mb-6">
          {[0, 1, 2, 3].map((s) => (
            <div
              key={s}
              className={`
                p-3 rounded-lg text-center
                ${s === seat ? 'bg-green-600 text-white' : 'bg-gray-700 text-gray-400'}
              `}
            >
              <div className="text-sm">{SEAT_NAMES[s]}家</div>
              <div className="text-lg">
                {s === seat ? '你' : '等待中...'}
              </div>
            </div>
          ))}
        </div>

        {/* 操作按钮 */}
        <div className="flex gap-4">
          <button
            className="flex-1 py-3 bg-gray-600 hover:bg-gray-500 text-white font-bold rounded-lg
                       transition-colors"
            onClick={onLeave}
          >
            离开
          </button>
          <button
            className="flex-1 py-3 bg-green-500 hover:bg-green-600 text-white font-bold rounded-lg
                       transition-colors"
            onClick={onReady}
          >
            准备 (开始游戏)
          </button>
        </div>

        {/* 说明 */}
        <p className="mt-4 text-gray-500 text-sm text-center">
          点击"准备"将用AI填充空位并开始游戏
        </p>
      </div>
    </div>
  );
};
