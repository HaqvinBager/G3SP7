#pragma once
#include <vector>
#include <memory>
#include "FModWrapper.h"
#include "Observer.h"

class CAudio;
class CAudioChannel;

enum class EMusic { Count };
enum class EAmbience { AirVent, Factory, Count };
enum class ESFX { GravityGlovePullBuildup, GravityGlovePullHit, GravityGlovePush, Count };
enum class ESFXCollection { StepAirVent, StepConcrete, Count };
enum class EUI { Count };
enum class EResearcherEventVoiceLine { ResearcherDoorEventVerticalSlice, ResearcherIntroVerticalSlice, Count };
enum class EResearcherReactionVoiceLine { ResearcherReactionExplosives, Count };
enum class ERobotVoiceLine { RobotAttack, RobotDeath, RobotIdle, RobotPatrolling, RobotSearching, Count };

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
	void PlayRandomSoundFromCollection(const std::vector<CAudio*>& aCollection, const EChannel& aChannel);

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
	std::vector<CAudio*> myRobotDeathSounds;
	std::vector<CAudio*> myRobotIdleSounds;
	std::vector<CAudio*> myRobotPatrollingSounds;
	std::vector<CAudio*> myRobotSearchingSounds;

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
