#pragma once

class IInputObserver {
public:
	enum class EInputAction
	{
		MouseLeftPressed,
		MouseLeftDown,
		MouseRight,
		MouseMiddle,
		KeyEscape,
		KeyEnter,
		KeyShiftDown,
		KeyShiftRelease,
		KeyW,
		KeyA,
		KeyS,
		KeyD,
		KeySpace
	};

	enum class EInputEvent
	{
		PopState,
		MoveClick,
		MoveDown,
		AttackClick,
		OpenMenuPress,
		LoadLevel,
		PauseGame,
		QuitGame,
		MiddleMouseMove,
		StandStill,
		Moving,
		MoveForward,
		MoveBackward,
		MoveLeft,
		MoveRight,
		Jump
	};

public:
	IInputObserver() = default;
	virtual ~IInputObserver() = default;
	virtual void ReceiveEvent(const EInputEvent aEvent) = 0;
};