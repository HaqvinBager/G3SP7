#pragma once
#pragma once
#include <vector>
#include <random>
#include <memory>
#include "FModWrapper.h"
#include "Observer.h"

class CAudio;
class CAudioChannel;

enum class EMusic { Count };
enum class EAmbience { AirVent, Factory, Count };
enum class ESFX { GravityGlovePullBuildup, GravityGlovePullHit, GravityGlovePush, StepAirVent, StepConcrete, Count };
enum class EUI { Count };
enum class EResearcherVoiceLine { ResearcherDoorEventVerticalSlice, ResearcherIntroVerticalSlice, ResearcherReactionExplosives, Count };
enum class ERobotVoiceLine { RobotAttack, RobotDeath, RobotIdle, RobotPatrolling, RobotSearching, Count };

enum class EChannels { Music, Ambience, SFX, UI, VOX, Count };

enum class GroundType;
enum class ESFXCollection;
class SFXCollection;

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

	// Listen to PostMaster
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
	std::string GetPath(EResearcherVoiceLine type) const;
	std::string GetPath(ERobotVoiceLine type) const;

	std::string TranslateChannels(EChannels enumerator) const;
	std::string TranslateMusic(EMusic enumerator) const;
	std::string TranslateAmbience(EAmbience enumerator) const;
	std::string TranslateSFX(ESFX enumerator) const;
	std::string TranslateUI(EUI enumerator) const;
	std::string TranslateVoiceLine(EResearcherVoiceLine enumerator) const;
	std::string TranslateVoiceLine(ERobotVoiceLine enumerator) const;

private:
	const std::string& myAmbiencePath = "Audio/Ambience/";
	const std::string& myMusicPath = "Audio/Music/";
	const std::string& mySFXPath = "Audio/SFX/";
	const std::string& myUIPath = "Audio/UI/";
	const std::string& myVoxPath = "Audio/VOX/";
	GroundType myCurrentGroundType;

	CFModWrapper myWrapper;

	std::vector<CAudio*> myAmbianceAudio;
	std::vector<CAudio*> myMusicAudio;
	std::vector<CAudio*> mySFXAudio;
	std::vector<CAudio*> myUIAudio;
	std::vector<CAudio*> myResearcherVoicelineAudio;
	std::vector<CAudio*> myRobotVoicelineAudio;

	std::vector<CAudioChannel*> myChannels;

	CAudio* mySound;
	CAudioChannel* myChannel;

	std::vector<EMusic> myMusics;

	struct SDelayedSFX
	{
		ESFX mySFX;
		float myTimer = 0.0f;
	};
	std::vector<SDelayedSFX> myDelayedSFX;
};

