#pragma once

namespace PostMaster
{
	struct SCrossHairData
	{
		unsigned int myIndex	= 0;
		bool myShouldLoop		= false; 
		bool myShouldBeReversed = false;
	};

	struct SGravityGloveTargetData
	{
		float myInitialDistanceSquared = 0.0f;
		float myCurrentDistanceSquared = 0.0f;
	};

	// SP7: Checks if the string is equivalent to any of our levels
	const bool LevelCheck(const char* aMessageToCompare);
}