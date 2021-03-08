#include "stdafx.h"
#include "FModWrapper.h"
#include "FMod.h"
#include "Audio.h"
#include "AudioChannel.h"

CFModWrapper::CFModWrapper()
{
	myFModInstance = new CFMod();
}

CFModWrapper::~CFModWrapper()
{
	delete myFModInstance;
	myFModInstance = nullptr;
}

CAudio* CFModWrapper::RequestSound(const std::string& aFilePath, bool aShouldLoop)
{
	CAudio* sound = new CAudio(myFModInstance->CreateSound(aFilePath, aShouldLoop));
	return std::move(sound);
}

CAudio* CFModWrapper::TryGetSound(const std::string& aFilePath, bool aShouldLoop)
{
	FMOD::Sound* sound = myFModInstance->TryCreateSound(aFilePath, aShouldLoop);
	
	if (sound)
	{
		CAudio* audio = new CAudio(sound);
		return std::move(audio);
	}

	return nullptr;
}

CAudioChannel* CFModWrapper::RequestChannel(const std::string& aChannelName)
{
	CAudioChannel* channel = new CAudioChannel(myFModInstance->CreateChannel(aChannelName));
	return std::move(channel);
}

void CFModWrapper::Play(CAudio* aSound, CAudioChannel* aChannel)
{
	myFModInstance->Play(aSound->myFModSound, aChannel->myFModChannel);
}
