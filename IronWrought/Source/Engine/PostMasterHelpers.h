#pragma once

namespace PostMaster
{
	static const char* SMSG_DISABLE_GLOVE  = "DisableGlove";
	static const char* SMSG_ENABLE_GLOVE   = "EnableGlove";
	static const char* SMSG_DISABLE_CANVAS = "DisableCanvas";
	static const char* SMSG_ENABLE_CANVAS  = "EnableCanvas";
	static const char* SMSG_PICKUP_GLOVE_EVENT = "GloveEvent";

	// SP7: compares a string received from SStringMessage.aMessageType with the levels we have.
	const bool LevelCheck(const char* aStringMessageToCheck);
	const bool DisableGravityGlove(const char* aStringMsg);
	const bool EnableGravityGlove(const char* aStringMsg);
	const bool DisableCanvas(const char* aStringMsg);
	const bool EnableCanvas(const char* aStringMsg);
	const bool GloveEvent(const char* aStringMsg);
	
}