import React from 'react';

interface ActionBarProps {
  canTsumo: boolean;
  canRon: boolean;
  canRiichi: boolean;
  canChi: boolean;
  canPon: boolean;
  canKan: boolean;
  onTsumo: () => void;
  onRon: () => void;
  onRiichi: () => void;
  onChi: () => void;
  onPon: () => void;
  onKan: () => void;
  onPass: () => void;
}

export const ActionBar: React.FC<ActionBarProps> = ({
  canTsumo,
  canRon,
  canRiichi,
  canChi,
  canPon,
  canKan,
  onTsumo,
  onRon,
  onRiichi,
  onChi,
  onPon,
  onKan,
  onPass,
}) => {
  const hasAnyAction = canTsumo || canRon || canRiichi || canChi || canPon || canKan;

  if (!hasAnyAction) {
    return null;
  }

  return (
    <div className="flex gap-2 p-4 bg-gray-800 rounded-lg">
      {canTsumo && (
        <ActionButton
          label="自摸"
          color="green"
          onClick={onTsumo}
        />
      )}
      {canRon && (
        <ActionButton
          label="荣和"
          color="green"
          onClick={onRon}
        />
      )}
      {canRiichi && (
        <ActionButton
          label="立直"
          color="yellow"
          onClick={onRiichi}
        />
      )}
      {canChi && (
        <ActionButton
          label="吃"
          color="blue"
          onClick={onChi}
        />
      )}
      {canPon && (
        <ActionButton
          label="碰"
          color="blue"
          onClick={onPon}
        />
      )}
      {canKan && (
        <ActionButton
          label="杠"
          color="purple"
          onClick={onKan}
        />
      )}
      <ActionButton
        label="过"
        color="gray"
        onClick={onPass}
      />
    </div>
  );
};

interface ActionButtonProps {
  label: string;
  color: 'green' | 'yellow' | 'blue' | 'purple' | 'gray' | 'red';
  onClick: () => void;
  disabled?: boolean;
}

const ActionButton: React.FC<ActionButtonProps> = ({
  label,
  color,
  onClick,
  disabled = false,
}) => {
  const colorClasses = {
    green: 'bg-green-500 hover:bg-green-600',
    yellow: 'bg-yellow-500 hover:bg-yellow-600 text-black',
    blue: 'bg-blue-500 hover:bg-blue-600',
    purple: 'bg-purple-500 hover:bg-purple-600',
    gray: 'bg-gray-500 hover:bg-gray-600',
    red: 'bg-red-500 hover:bg-red-600',
  };

  return (
    <button
      className={`
        px-6 py-3 rounded-lg font-bold text-lg
        ${colorClasses[color]}
        text-white transition-colors
        disabled:opacity-50 disabled:cursor-not-allowed
      `}
      onClick={onClick}
      disabled={disabled}
    >
      {label}
    </button>
  );
};
