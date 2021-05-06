#include "stdafx.h"
#include "PostMasterHelpers.h"

const bool PostMaster::LevelCheck(const char* aStringMessageToCheck)
{
	{
		std::array<const char*, 4> levels = { "Level_1-1", "Level_1-2", "Level_2-1", "Level_2-2" };
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
