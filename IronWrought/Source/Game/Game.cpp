#include "stdafx.h"
#include "Game.h"

#ifdef _DEBUG
#pragma comment(lib, "Engine_Debug.lib")
#endif // _DEBUG
#ifdef NDEBUG
#pragma comment(lib, "Engine_Release.lib")
#endif // NDEBUG

CGame::CGame()
{
}

CGame::~CGame()
{
}

void CGame::Init()
{
	InitDev();
}

bool CGame::Update()
{
	bool stateStackHasUpdated = myStateStack.Update();
	return stateStackHasUpdated;
}

void CGame::InitDev()
{
	myStateStack.Awake(
		{
			CStateStack::EState::GameWorld
		},
		CStateStack::EState::GameWorld);
}

void CGame::InitRealGame()
{
	// Currently we wish to only have an InGame and PauseMenu state (preprod - 05 JAN)
	myStateStack.Awake(
		{
		CStateStack::EState::InGame
		},
		CStateStack::EState::InGame);
}
