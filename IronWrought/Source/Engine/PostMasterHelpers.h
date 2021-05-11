#pragma once

namespace PostMaster
{
	static const char* SMSG_DISABLE_GLOVE  = "DisableGlove";
	static const char* SMSG_ENABLE_GLOVE   = "EnableGlove";
	static const char* SMSG_DISABLE_CANVAS = "DisableCanvas";
	static const char* SMSG_ENABLE_CANVAS  = "EnableCanvas";
	static const char* SMSG_PICKUP_GLOVE_EVENT = "GloveEvent";
	static const char* SMSG_FIRST_END_EVENT = "EndEventFirst";

	static const char* SMSG_OUTRO1 = "Outro1";
	static const char* SMSG_OUTRO2 = "Outro2";
	static const char* SMSG_OUTRO3 = "Outro3";
	static const char* SMSG_OUTRO4 = "Outro4";
	static const char* SMSG_OUTRO5 = "Outro5";
	static const char* SMSG_OUTRO6 = "Outro6";
	static const char* SMSG_OUTRO7 = "Outro7";

	// SP7: compares a string received from SStringMessage.aMessageType with the levels we have.
	const bool LevelCheck(const char* aStringMessageToCheck);
	const bool DisableGravityGlove(const char* aStringMsg);
	const bool EnableGravityGlove(const char* aStringMsg);
	const bool DisableCanvas(const char* aStringMsg);
	const bool EnableCanvas(const char* aStringMsg);
	const bool GloveEvent(const char* aStringMsg);
	const bool EndEventFirst(const char* aStringMsg);
	const bool CompareStringMessage(const char* aCompareTo, const char* aReceived);
	
}