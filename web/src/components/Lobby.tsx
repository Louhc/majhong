import React, { useState } from 'react';

interface LobbyProps {
  connected: boolean;
  onCreateRoom: () => void;
  onJoinRoom: (roomId: string) => void;
}

export const Lobby: React.FC<LobbyProps> = ({
  connected,
  onCreateRoom,
  onJoinRoom,
}) => {
  const [roomIdInput, setRoomIdInput] = useState('');

  const handleJoinRoom = () => {
    if (roomIdInput.trim()) {
      onJoinRoom(roomIdInput.trim().toUpperCase());
    }
  };

  return (
    <div className="min-h-screen flex items-center justify-center bg-gradient-to-b from-gray-900 to-gray-800">
      <div className="bg-gray-800 p-8 rounded-xl shadow-2xl w-full max-w-md">
        <h1 className="text-4xl font-bold text-white text-center mb-2">
          🀄 麻将游戏
        </h1>
        <p className="text-gray-400 text-center mb-8">
          日本麻将 (立直)
        </p>

        {/* 连接状态 */}
        <div className="flex items-center justify-center gap-2 mb-8">
          <div className={`w-3 h-3 rounded-full ${connected ? 'bg-green-500' : 'bg-red-500'}`} />
          <span className={connected ? 'text-green-400' : 'text-red-400'}>
            {connected ? '已连接' : '未连接'}
          </span>
        </div>

        {/* 创建房间 */}
        <button
          className="w-full py-4 bg-green-500 hover:bg-green-600 text-white font-bold text-lg rounded-lg
                     transition-colors disabled:opacity-50 disabled:cursor-not-allowed mb-4"
          onClick={onCreateRoom}
          disabled={!connected}
        >
          创建房间
        </button>

        {/* 分隔线 */}
        <div className="flex items-center gap-4 my-6">
          <div className="flex-1 h-px bg-gray-600" />
          <span className="text-gray-500">或</span>
          <div className="flex-1 h-px bg-gray-600" />
        </div>

        {/* 加入房间 */}
        <div className="space-y-4">
          <input
            type="text"
            placeholder="输入房间号"
            value={roomIdInput}
            onChange={(e) => setRoomIdInput(e.target.value)}
            className="w-full px-4 py-3 bg-gray-700 text-white rounded-lg border border-gray-600
                       focus:border-blue-500 focus:outline-none placeholder-gray-400
                       uppercase"
            maxLength={10}
          />
          <button
            className="w-full py-4 bg-blue-500 hover:bg-blue-600 text-white font-bold text-lg rounded-lg
                       transition-colors disabled:opacity-50 disabled:cursor-not-allowed"
            onClick={handleJoinRoom}
            disabled={!connected || !roomIdInput.trim()}
          >
            加入房间
          </button>
        </div>

        {/* 说明 */}
        <div className="mt-8 text-gray-500 text-sm text-center">
          <p>创建房间后，邀请好友输入房间号加入</p>
          <p>不足4人时将自动填充AI玩家</p>
        </div>
      </div>
    </div>
  );
};
