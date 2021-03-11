#pragma once
#include <string>

class CFMod;
class CAudio;
class CAudioChannel;

class CFModWrapper
{
public:
	CFModWrapper();
	~CFModWrapper();

	CAudio* RequestSound(const std::string& aFilePath, bool aShouldLoop = false);
	CAudio* TryGetSound(const std::string& aFilePath, bool aShouldLoop = false);

	CAudioChannel* RequestChannel(const std::string& aChannelName);

	void Play(CAudio* aSound, CAudioChannel* aChannel);

private:
	
private:
	CFMod* myFModInstance;
};

