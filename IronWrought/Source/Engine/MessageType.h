#pragma once
enum class EMessageType
{
	ColliderAdded,
	ColliderRemoved,

	MainMenu,
	Credits,
	Options,
	LoadLevel,
	StartGame,
	LevelSelect,
	Quit,
	Resume,

	PlayerHealthChanged,
	PlayerStaminaChanged,
	PlayerTakeDamage,
	PlayerDied,
	EnemyDied,
	EnemyHealthChanged,

	IntroStarted,
	IntroEnded,

	UIButtonPress,

	PlayVoiceLine,
	LoadDialogue,
	StopDialogue,

	FadeInComplete,
	FadeOutComplete,

	SetResolution1280x720,
	SetResolution1600x900,
	SetResolution1920x1080,

	Count
};
