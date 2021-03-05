#include "stdafx.h"
#include <iostream>
#include <random>
#include "AudioManager.h"
#include "Audio.h"
#include "AudioChannel.h"
#include "MainSingleton.h"
#include "PostMaster.h"
#include "JsonReader.h"

/*////////////////////////////////////
* Suggestion
*	Create separate postmaster for sounds (AudioPostMaster)
*   Create separate enum list for messages:
*		EMusic
*		ESFX
*		EVox
*		...
* 
*	For a less bloated Receive() and also to reduce clutter in main PostMaster
* 
*////////////////////////////////////

using namespace rapidjson;

#define CAST(type) { static_cast<unsigned int>(type) }

CAudioManager::CAudioManager() : myWrapper() {
	SubscribeToMessages();

	Document document = CJsonReader::Get()->LoadDocument("Json/Audio/AudioPaths.json");

	if (document.HasParseError()) { return; }

	// Init Channels
	for (unsigned int i = 0; i < static_cast<unsigned int>(EChannels::Count); ++i) {
		myChannels.push_back(myWrapper.RequestChannel(TranslateChannels(static_cast<EChannels>(i))));
	}
//#pragma region AudioLoading
//
//	if (document.HasMember("Ambience"))
//	{
//		auto audioDataArray = document["Ambience"].GetArray();
//		for (unsigned int i = 0; i < audioDataArray.Size(); ++i)
//		{
//			auto audioData = audioDataArray[i].GetObjectW();
//			myAmbianceAudio.push_back(myWrapper.RequestSound(myAmbiencePath + audioData["Path"].GetString()));
//		}
//	}
//
//	if (document.HasMember("Music"))
//	{
//		auto audioDataArray = document["Music"].GetArray();
//		for (unsigned int i = 0; i < audioDataArray.Size(); ++i)
//		{
//			auto audioData = audioDataArray[i].GetObjectW();
//			myMusicAudio.push_back(myWrapper.RequestLoopingSound(myMusicPath + audioData["Path"].GetString()));
//		}
//	}
//
//	if (document.HasMember("SFX"))
//	{
//		auto audioDataArray = document["SFX"].GetArray();
//		for (unsigned int i = 0; i < audioDataArray.Size(); ++i)
//		{
//			auto audioData = audioDataArray[i].GetObjectW();
//			mySFXAudio.push_back(myWrapper.RequestSound(mySFXPath + audioData["Path"].GetString()));
//		}
//	}
//
//	if (document.HasMember("UI"))
//	{
//		auto audioDataArray = document["UI"].GetArray();
//		for (unsigned int i = 0; i < audioDataArray.Size(); ++i)
//		{
//			auto audioData = audioDataArray[i].GetObjectW();
//			myUIAudio.push_back(myWrapper.RequestSound(myUIPath + audioData["Path"].GetString()));
//		}
//	}
//
//	if (document.HasMember("VoiceLine"))
//	{
//		auto audioDataArray = document["VoiceLine"].GetArray();
//		for (unsigned int i = 0; i < audioDataArray.Size(); ++i)
//		{
//			auto audioData = audioDataArray[i].GetObjectW();
//			const std::string& path = audioData["Path"].GetString();
//			if (path.find("Researcher"))
//				myResearcherVoicelineAudio.push_back(myWrapper.RequestSound(myVoxPath + path));
//			if (path.find("Robot"))
//				myRobotVoicelineAudio.push_back(myWrapper.RequestSound(myVoxPath + path));
//		}
//	}
//
//#pragma endregion

	std::ifstream volumeStream("Json/Audio/AudioVolume.json");
	IStreamWrapper volumeWrapper(volumeStream);
	Document volDoc;
	volDoc.ParseStream(volumeWrapper);
	
	if (volDoc.HasParseError()) { return; }
	
	if (volDoc.HasMember("Ambience"))
	{
		float value = volDoc["Ambience"].GetFloat();
		myChannels[CAST(EChannels::Ambience)]->SetVolume(value);
	}
	if (volDoc.HasMember("Music"))
	{
		float value = volDoc["Music"].GetFloat();
		myChannels[CAST(EChannels::Music)]->SetVolume(value);
	}
	if (volDoc.HasMember("SFX"))
	{
		float value = volDoc["SFX"].GetFloat();
		myChannels[CAST(EChannels::SFX)]->SetVolume(value);
	}
	if (volDoc.HasMember("UI"))
	{
		float value = volDoc["UI"].GetFloat();
		myChannels[CAST(EChannels::UI)]->SetVolume(value);
	}
	if (volDoc.HasMember("Voice"))
	{
		float value = volDoc["Voice"].GetFloat();
		myChannels[CAST(EChannels::VOX)]->SetVolume(value);
	}
}

	CAudioManager::~CAudioManager()
	{
		UnsubscribeToMessages();
		// 2020 12 06 - CAudio attempts to delete myFModSound, seems to be shared. 
		//for (auto& channel : myChannels)
		//{
		//	delete channel;
		//	channel = nullptr;
		//}
		//for (auto& music : myMusicAudio)
		//{
		//	delete music;
		//	music = nullptr;
		//}
		//for (auto& ambience : myAmbianceAudio)
		//{
		//	delete ambience;
		//	ambience = nullptr;
		//}
		//for (auto& sfx : mySFXAudio)
		//{
		//	delete sfx;
		//	sfx = nullptr;
		//}
		//for (auto& ui : myUIAudio)
		//{
		//	delete ui;
		//	ui = nullptr;
		//}
		//for (auto& voice : myVoicelineAudio)
		//{
		//	delete voice;
		//	voice = nullptr;
		//}
	}


void CAudioManager::Receive(const SMessage& aMessage) {
	switch (aMessage.myMessageType)
	{
	// UI
	case EMessageType::UIButtonPress:
	{
		//if (myUIAudio.size() >= static_cast<unsigned int>(EUI::ButtonClick))
		//{
		//	myWrapper.Play(myUIAudio[CAST(EUI::ButtonClick)], myChannels[CAST(EChannels::UI)]);
		//}
	}break;

	// VOICELINES
	case EMessageType::PlayVoiceLine:
	{
		//if (!myVoicelineAudio.empty()) {
		//	int index = *static_cast<int*>(aMessage.data);
		//	myChannels[CAST(EChannels::VOX)]->Stop();
		//	myWrapper.Play(myVoicelineAudio[index], myChannels[CAST(EChannels::VOX)]);
		//}
	}break;

	case EMessageType::StopDialogue:
	{
		//if (!myVoicelineAudio.empty()) {
		//	myChannels[CAST(EChannels::VOX)]->Stop();
		//}
	}break;

	default: break;
	}
}

void CAudioManager::Receive(const SStringMessage& /*aMessage*/)
{}

void CAudioManager::Update()
{
	if (myDelayedSFX.size() > 0)
	{
		const float dt = CTimer::Dt();

		for (auto it = myDelayedSFX.begin(); it != myDelayedSFX.end();)
		{
			it->myTimer -= dt;
			if (it->myTimer <= 0.0f)
			{
				myWrapper.Play(mySFXAudio[CAST(it->mySFX)], myChannels[CAST(EChannels::SFX)]);
				it = myDelayedSFX.erase(it);
				continue;
			}
			++it;
		}
	}
}

void CAudioManager::SubscribeToMessages()
{
	CMainSingleton::PostMaster().Subscribe(EMessageType::UIButtonPress, this);

	CMainSingleton::PostMaster().Subscribe(EMessageType::PlayVoiceLine, this);
	CMainSingleton::PostMaster().Subscribe(EMessageType::StopDialogue, this);
}

void CAudioManager::UnsubscribeToMessages()
{
	CMainSingleton::PostMaster().Unsubscribe(EMessageType::UIButtonPress, this);

	CMainSingleton::PostMaster().Unsubscribe(EMessageType::PlayVoiceLine, this);
	CMainSingleton::PostMaster().Unsubscribe(EMessageType::StopDialogue, this);
}

std::string CAudioManager::GetPath(EMusic type) const
{
	std::string path = myMusicPath;
	path.append(TranslateMusic(type));
	path.append(".mp3");
	return path;
}

std::string CAudioManager::GetPath(EAmbience type) const
{
	std::string path = myAmbiencePath;
	path.append(TranslateAmbience(type));
	path.append(".mp3");
	return path;
}

std::string CAudioManager::GetPath(ESFX type) const
{
	std::string path = mySFXPath;
	path.append(TranslateSFX(type));
	path.append(".mp3");
	return path;
}

std::string CAudioManager::GetPath(EUI type) const
{
	std::string path = myUIPath;
	path.append(TranslateUI(type));
	path.append(".mp3");
	return path;
}

std::string CAudioManager::GetPath(EResearcherVoiceLine type) const
{
	std::string path = myVoxPath;
	path.append(TranslateVoiceLine(type));
	path.append(".mp3");
	return path;
}

std::string CAudioManager::GetPath(ERobotVoiceLine type) const
{
	std::string path = myVoxPath;
	path.append(TranslateVoiceLine(type));
	path.append(".mp3");
	return path;
}

std::string CAudioManager::TranslateChannels(EChannels enumerator) const
{
	switch (enumerator)
	{
	case EChannels::Music:
		return "Music";
	case EChannels::Ambience:
		return "Ambience";
	case EChannels::SFX:
		return "SFX";
	case EChannels::UI:
		return "UI";
	case EChannels::VOX:
		return "VOX";
	default:
		return "";
	}
}

std::string CAudioManager::TranslateMusic(EMusic /*enumerator*/) const
{
	//switch (enumerator) {
	//default:
	//	return "";
	//}
	return "DE";
}

std::string CAudioManager::TranslateAmbience(EAmbience enumerator) const {
	switch (enumerator)
	{
	case EAmbience::AirVent:
		return "AirVent";
	case EAmbience::Factory:
		return "Factory";
	default:
		return "";
	}
}
std::string CAudioManager::TranslateSFX(ESFX enumerator) const {
	switch (enumerator)
	{
	case ESFX::GravityGlovePullBuildup:
		return "GravityGlovePullBuildup";
	case ESFX::GravityGlovePullHit:
		return "GravityGlovePullHit";
	case ESFX::GravityGlovePush:
		return "GravityGlovePush";
	case ESFX::StepAirVent:
		return "StepAirVent";
	case ESFX::StepConcrete:
		return "StepConcrete";
	default:
		return "";
	}
	
}
std::string CAudioManager::TranslateUI(EUI /*enumerator*/) const {
	//switch (enumerator) {
	////case EUI::ButtonClick:
	////	return "ButtonClick";
	//default:
	//	return "";
	//}
	return "";
}
std::string CAudioManager::TranslateVoiceLine(EResearcherVoiceLine enumerator) const
{
	switch (enumerator)
	{
	case EResearcherVoiceLine::ResearcherDoorEventVerticalSlice:
		return "ResearcherDoorEventVerticalSlice";
	case EResearcherVoiceLine::ResearcherIntroVerticalSlice:
		return "ResearcherIntroVerticalSlice";
	case EResearcherVoiceLine::ResearcherReactionExplosives:
		return "ResearcherReactionExplosives";
	default:
		return "";
	}
}
std::string CAudioManager::TranslateVoiceLine(ERobotVoiceLine enumerator) const
{
	switch (enumerator)
	{
	case ERobotVoiceLine::RobotAttack:
		return "RobotAttack";
	case ERobotVoiceLine::RobotDeath:
		return "RobotDeath";
	case ERobotVoiceLine::RobotIdle:
		return "RobotIdle";
	case ERobotVoiceLine::RobotPatrolling:
		return "RobotPatrolling";
	case ERobotVoiceLine::RobotSearching:
		return "RobotSearching";
	default:
		return "";
	}
}
