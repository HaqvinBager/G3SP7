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
	PlayerResourceChanged,
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
	SetResolution2560x1440,

	PlayStepSound,
	PlayResearcherReactionExplosives,
	PlayRobotAttackSound,
	PlayRobotDeathSound,
	PlayRobotIdleSound,
	PlayRobotPatrolling,
	PlayRobotSearching,
	PlayResearcherEvent,
	PlaySFX,

	CursorHideAndLock,
	CursorShowAndUnlock,

	ComponentAdded,

	UpdateCrosshair,

	CameraShake,

	ToggleWidget,

	LockPlayer,

	GravityGloveTargetDistance,// Specific, solves single problem. Remove later / Aki 2021 04 29
	GravityGlovePull,
	GravityGlovePush,

	PlayerHealthPickup,
	PlayerSetRespawnPoint,
	PlayerRespawn,

	EnemyPatrolState,
	EnemySeekState,
	EnemyAttackState,
	EnemyAttack,
	EnemyTargetLost,
	EnemyTakeDamage,
	EnemyDisabled,

	BootUpState,

	Count
};
