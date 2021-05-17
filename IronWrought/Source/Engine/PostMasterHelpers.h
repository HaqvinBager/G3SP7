#pragma once

namespace PostMaster
{
	static const char* SMSG_DISABLE_GLOVE  = "DisableGlove";
	static const char* SMSG_ENABLE_GLOVE   = "EnableGlove";
	static const char* SMSG_DISABLE_CANVAS = "DisableCanvas";
	static const char* SMSG_ENABLE_CANVAS  = "EnableCanvas";
	static const char* SMSG_PICKUP_GLOVE_EVENT = "GloveEvent";
	static const char* SMSG_TO_MAIN_MENU = "ToMainMenu";

	static const char* SMSG_OUTRO1 = "Outro1";
	static const char* SMSG_OUTRO2 = "Outro2";
	static const char* SMSG_OUTRO3 = "Outro3";
	static const char* SMSG_OUTRO4 = "Outro4";
	static const char* SMSG_OUTRO5 = "Outro5";
	static const char* SMSG_OUTRO6 = "Outro6";
	static const char* SMSG_OUTRO7 = "Outro7";

	static const char* SMSG_INTRO = "Intro";

	static const char* SMSG_LEVEL11 = "Level_1-1";
	static const char* SMSG_LEVEL12 = "Level_1-2";
	static const char* SMSG_LEVEL21 = "Level_2-1";
	static const char* SMSG_LEVEL22 = "Level_2-2";

	static const char* SMSG_AUDIO = "AudioEvent";

	static const char* SMSG_UIMOVE = "UIMove";
	static const char* SMSG_UIINTERACT = "UIInteract";
	static const char* SMSG_UIPULL = "UIPull";
	static const char* SMSG_UIPUSH = "UIPush";
	static const char* SMSG_UICROUCH = "UICrouch";
	static const char* SMSG_UIJUMP = "UIJump";

	// SP7: compares a string received from SStringMessage.aMessageType with the levels we have.
	const bool LevelCheck(const char* aStringMessageToCheck);

	const bool DisableGravityGlove(const char* aStringMsg);
	const bool EnableGravityGlove(const char* aStringMsg);
	const bool DisableCanvas(const char* aStringMsg);
	const bool EnableCanvas(const char* aStringMsg);
	const bool GloveEvent(const char* aStringMsg);

	const bool CompareStringMessage(const char* aCompareTo, const char* aReceived);
	
}