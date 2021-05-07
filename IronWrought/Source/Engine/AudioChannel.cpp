#include "stdafx.h"
#include "AudioChannel.h"
#include <fmod/fmod.hpp>

const bool CAudioChannel::IsPlaying() const
{
    bool isPlaying = false;
    myFModChannel->isPlaying(&isPlaying);
    return isPlaying;
}

const int CAudioChannel::PlayCount() const
{
    int count = 0;
    myFModChannel->getNumChannels(&count);
    return count;
}

void CAudioChannel::SetVolume(float aVolumeLevel)
{
    myFModChannel->setVolume(aVolumeLevel);
}

void CAudioChannel::Stop()
{
    myFModChannel->stop();
}

void CAudioChannel::Mute(bool aDoMute)
{
    myFModChannel->setMute(aDoMute);
}

CAudioChannel::CAudioChannel(FMOD::ChannelGroup* aChannelPointer)
{
    myFModChannel = aChannelPointer;
}

CAudioChannel::~CAudioChannel()
{
    delete myFModChannel;
    myFModChannel = nullptr;
}
