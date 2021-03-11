#pragma once
#include <string>
#include <fmod/fmod_common.h>

namespace FMOD {
	namespace Studio {
		class System;
	}
	class System;
	class Sound;
	class ChannelGroup;
}

class CFMod
{
public:
	CFMod();
	~CFMod();

	const FMOD::Studio::System* GetStudioSystem() const;
	const FMOD::System* GetCoreSystem() const;

	FMOD::Sound* CreateSound(const std::string& aFilePath, bool aShouldLoop = false);
	FMOD::Sound* TryCreateSound(const std::string& aFilePath, bool aShouldLoop = false);
	FMOD::ChannelGroup* CreateChannel(const std::string& aChannelName);

	void Play(FMOD::Sound* aSound, FMOD::ChannelGroup* aChannelGroup);

private:
	static void CheckException(FMOD_RESULT aResult);

private:
	FMOD::Studio::System* myStudioSystem;
	FMOD::System* myCoreSystem;

};
