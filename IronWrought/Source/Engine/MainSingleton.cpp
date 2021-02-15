#include "stdafx.h"
#include "MainSingleton.h"
#include "CollisionManager.h"
#include "PopupTextService.h"
#include "DialogueSystem.h"
#include "PopupTextService.h"
#include "MaterialHandler.h"
#include "ImguiManager.h"

CMainSingleton* CMainSingleton::ourInstance = nullptr;

CMainSingleton::CMainSingleton() {
	ourInstance = this;
	myPostMaster = new CPostMaster();
	myCollisionManager = new CCollisionManager();
	myPopupTextService = new CPopupTextService();
	myDialogueSystem = new CDialogueSystem();
	myMaterialHandler = new CMaterialHandler();
	myImguiManager = new CImguiManager();
}

CMainSingleton::~CMainSingleton()
{
	SAFE_DELETE(myPostMaster);
	SAFE_DELETE(myCollisionManager);
	SAFE_DELETE(myPopupTextService);
	SAFE_DELETE(myDialogueSystem);
	SAFE_DELETE(myMaterialHandler);
	SAFE_DELETE(myImguiManager);
	ourInstance = nullptr;
}

CPostMaster& CMainSingleton::PostMaster()
{
	return *ourInstance->myPostMaster;
}

CCollisionManager & CMainSingleton::CollisionManager()
{
	return *ourInstance->myCollisionManager;
}

CPopupTextService& CMainSingleton::PopupTextService()
{
	return *ourInstance->myPopupTextService;
}

CDialogueSystem& CMainSingleton::DialogueSystem()
{
	return *ourInstance->myDialogueSystem;
}

CMaterialHandler& CMainSingleton::MaterialHandler()
{
	return *ourInstance->myMaterialHandler;
}

CImguiManager& CMainSingleton::ImguiManager()
{
	return *ourInstance->myImguiManager;
}
