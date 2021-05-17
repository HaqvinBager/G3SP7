#pragma once
#include <vector>
#include <memory>
#include "FModWrapper.h"
#include "Observer.h"

class CAudio;
class CAudioChannel;

enum class EMusic { Count };
enum class EAmbience { AirVent, Inside, Outside, Count };
enum class ESFX { GravityGlovePullBuildup, GravityGlovePullHit, GravityGlovePush, GravityGlovePullRelease, Jump, EnemyHit, SwitchPress, PickupGravityGlove, PickupHeal, EnemyAttack, Count };
enum class ESFXCollection { StepAirVent, StepConcrete, Count };
enum class EUI { ButtonClick, PlayClick, Count };
enum class EResearcherEventVoiceLine { V1, V2, BootUp, Intro, Line1, Line2, Line3, Line4, Line5, Line6, Line7, Line8, Line9, Line10, Line11, Line12, Line13, Line14, Line15, Line16, Line17, Line18, Line19, Line20, Line21, Line22, Line23, Line24, Line25, Line26, Line27, Outro1, Outro2, Outro3, Outro4, Outro5, Outro6, Outro7, Count };
enum class EResearcherReactionVoiceLine { ResearcherReactionExplosives, Count };
enum class ERobotVoiceLine { RobotAttack, RobotDeath, RobotIdle, RobotPatrolling, RobotSearching, RobotDamage, Count };

enum class EChannel { Music, Ambience, SFX, UI, ResearcherVOX, RobotVOX, Count };

enum class EGroundType { Concrete, AirVent, Count };

enum class SoundChannels {
	EMusic,
	EAmbience,
	ESFX,
	EUI,
	EResearcherVoiceLine,
	ERobotVoiceLine,
	Count
};

class CAudioManager : public IObserver, public IStringObserver {
public:
	CAudioManager();
	~CAudioManager();

	void Receive(const SMessage& aMessage);
	void Receive(const SStringMessage& aMessage);

	void Update();

private:
	void SubscribeToMessages();
	void UnsubscribeToMessages();

	std::string GetPath(EMusic type) const;
	std::string GetPath(EAmbience type) const;
	std::string GetPath(ESFX type) const;
	std::string GetPath(EUI type) const;
	std::string GetPath(EResearcherEventVoiceLine type) const;

	template <class T>
	std::string GetCollectionPath(T enumerator, unsigned int anIndex) const;

	std::string TranslateEnum(EChannel enumerator) const;
	std::string TranslateEnum(EMusic enumerator) const;
	std::string TranslateEnum(EAmbience enumerator) const;
	std::string TranslateEnum(ESFX enumerator) const;
	std::string TranslateEnum(ESFXCollection enumerator) const;
	std::string TranslateEnum(EUI enumerator) const;
	std::string TranslateEnum(EResearcherEventVoiceLine enumerator) const;
	std::string TranslateEnum(EResearcherReactionVoiceLine enumerator) const;
	std::string TranslateEnum(ERobotVoiceLine enumerator) const;

	void FillCollection(ESFXCollection enumerator);
	void FillCollection(EResearcherReactionVoiceLine enumerator);
	void FillCollection(ERobotVoiceLine enumerator);

private:
	void PlayRandomSoundFromCollection(const std::vector<CAudio*>& aCollection, const EChannel& aChannel, const int& aMaxNrOfChannelsActive = 5);
	void PlayCyclicRandomSoundFromCollection(const std::vector<CAudio*>& aCollection, const EChannel& aChannel, std::vector<int>& someCollectionIndices, const int& aMaxNrOfChannelsActive = 5);

private:
	const std::string& myAmbiencePath = "Audio/Ambience/";
	const std::string& myMusicPath = "Audio/Music/";
	const std::string& mySFXPath = "Audio/SFX/";
	const std::string& myUIPath = "Audio/UI/";
	const std::string& myVoxPath = "Audio/VOX/";
	
	EGroundType myCurrentGroundType;

	CFModWrapper myWrapper;

	std::vector<CAudio*> myAmbienceAudio;
	std::vector<CAudio*> myMusicAudio;
	std::vector<CAudio*> mySFXAudio;
	std::vector<CAudio*> myUIAudio;

	std::vector<CAudio*> myAirVentStepSounds;
	std::vector<CAudio*> myConcreteStepSounds;

	std::vector<CAudio*> myResearcherEventSounds;
	std::vector<CAudio*> myResearcherReactionsExplosives;

	std::vector<CAudio*> myRobotAttackSounds;
	std::vector<CAudio*> myRobotDamageSounds;
	std::vector<CAudio*> myRobotDeathSounds;
	std::vector<CAudio*> myRobotIdleSounds;
	std::vector<CAudio*> myRobotPatrollingSounds;
	std::vector<CAudio*> myRobotSearchingSounds;

	std::vector<int> myStepSoundIndices;
	std::vector<int> myAttackSoundIndices;
	std::vector<int> myDamageSoundIndices;
	std::vector<int> myDeathSoundIndices;
	std::vector<int> myIdleSoundIndices;
	std::vector<int> myPatrollingSoundIndices;
	std::vector<int> mySearchingSoundIndices;

	std::vector<CAudioChannel*> myChannels;

	struct SDelayedSFX
	{
		ESFX mySFX;
		float myTimer = 0.0f;
	};
	std::vector<SDelayedSFX> myDelayedSFX;
};

template<class T>
inline std::string CAudioManager::GetCollectionPath(T enumerator, unsigned int anIndex) const
{
	std::string path = "";
	path.append(TranslateEnum(enumerator));
	path.append(std::to_string(anIndex));
	path.append(".mp3");
	return path;
}
