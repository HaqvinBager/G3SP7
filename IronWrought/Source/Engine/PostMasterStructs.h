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
}