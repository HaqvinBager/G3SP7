#include "stdafx.h"
#include "PostMasterHelpers.h"

const bool PostMaster::LevelCheck(const char* aStringMessageToCheck)
{
	{
		std::array<const char*, 4> levels = { SMSG_LEVEL11, SMSG_LEVEL12, SMSG_LEVEL21, SMSG_LEVEL22 };
		for (auto& level : levels)
		{
			if (strcmp(aStringMessageToCheck, level) == 0)
			{
				return true;
			}
		}
		return false;
	}
}

const bool PostMaster::DisableGravityGlove(const char* aStringMsg)
{
	return strcmp(aStringMsg, SMSG_DISABLE_GLOVE) == 0;
}

const bool PostMaster::EnableGravityGlove(const char* aStringMsg)
{
	return strcmp(aStringMsg, SMSG_ENABLE_GLOVE) == 0;
}
const bool PostMaster::DisableCanvas(const char* aStringMsg)
{
	return strcmp(aStringMsg, SMSG_DISABLE_CANVAS) == 0;
}
const bool PostMaster::EnableCanvas(const char* aStringMsg)
{
	return strcmp(aStringMsg, SMSG_ENABLE_CANVAS) == 0;
}

const bool PostMaster::GloveEvent(const char* aStringMsg)
{
	return strcmp(aStringMsg, SMSG_PICKUP_GLOVE_EVENT) == 0;
}

const bool PostMaster::CompareStringMessage(const char* aCompareTo, const char* aReceived)
{
	return strcmp(aCompareTo, aReceived) == 0;
}
