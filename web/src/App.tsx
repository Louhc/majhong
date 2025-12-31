import { useGame } from './hooks/useGame';
import { Lobby } from './components/Lobby';
import { WaitingRoom } from './components/WaitingRoom';
import { GamePage } from './components/GamePage';

// WebSocket 服务器地址
const WS_URL = import.meta.env.VITE_WS_URL || 'ws://localhost:8080/ws';

function App() {
  const {
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
  } = useGame(WS_URL);

  // 根据游戏阶段渲染不同页面
  switch (phase) {
    case 'lobby':
      return (
        <Lobby
          connected={connected}
          onCreateRoom={createRoom}
          onJoinRoom={joinRoom}
        />
      );

    case 'waiting':
      return (
        <WaitingRoom
          roomId={roomId || ''}
          seat={seat}
          onReady={ready}
          onLeave={leaveRoom}
        />
      );

    case 'playing':
    case 'finished':
      return (
        <GamePage
          gameState={gameState}
          hand={hand}
          seat={seat}
          availableActions={availableActions}
          onDiscard={discard}
          onTsumo={declareWin}
          onRon={declareWin}
          onRiichi={declareRiichi}
          onChi={declareChi}
          onPon={declarePon}
          onKan={declareKan}
          onPass={pass}
          messages={messages}
        />
      );

    default:
      return null;
  }
}

export default App;
