#pragma once

namespace PostMaster
{
	// SP7: compares a string received from SStringMessage.aMessageType with the levels we have.
	const bool LevelCheck(const char* aStringMessageToCheck);
}