#pragma once

class IInputObserver {
public:
	enum class EInputAction
	{
		MouseLeftPressed,
		MouseLeftDown,
		MouseRightDown,
		MouseMiddle,
		KeyEscape,
		KeyEnter,
		KeyShiftDown,
		KeyShiftRelease,
		KeyW,
		KeyA,
		KeyS,
		KeyD,
		KeySpace,
		CTRL,
		KeyF5,
		KeyF8
	};

	enum class EInputEvent
	{
		PopState,
		Push,
		Pull,
		MoveDown,
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
		Jump,
		Crouch,
		ResetEntities,
		SetResetPointEntities
	};

public:
	IInputObserver() = default;
	virtual ~IInputObserver() = default;
	virtual void ReceiveEvent(const EInputEvent aEvent) = 0;
};