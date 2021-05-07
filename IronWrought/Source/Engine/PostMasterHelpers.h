#pragma once

namespace PostMaster
{
	static const char* MSG_DISABLE_GLOVE = "DisableGlove";
	static const char* MSG_ENABLE_GLOVE  = "EnableGlove";

	// SP7: compares a string received from SStringMessage.aMessageType with the levels we have.
	const bool LevelCheck(const char* aStringMessageToCheck);
	const bool DisableGravityGlove(const char* aStringMsg);
	const bool EnableGravityGlove(const char* aStringMsg);
}