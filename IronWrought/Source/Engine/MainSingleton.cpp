#include "stdafx.h"
#include "MainSingleton.h"
#include "CollisionManager.h"
#include "PopupTextService.h"
#include "DialogueSystem.h"
#include "PopupTextService.h"
#include "PhysXWrapper.h"
#include "MaterialHandler.h"

CMainSingleton* CMainSingleton::ourInstance = nullptr;

CMainSingleton::CMainSingleton() {
	ourInstance = this;
	myPostMaster = new CPostMaster();
	myCollisionManager = new CCollisionManager();
	myPopupTextService = new CPopupTextService();
	myDialogueSystem = new CDialogueSystem();
	myPhysXWrapper = new CPhysXWrapper();
	myMaterialHandler = new CMaterialHandler();
}

CMainSingleton::~CMainSingleton()
{
	SAFE_DELETE(myPostMaster);
	SAFE_DELETE(myCollisionManager);
	SAFE_DELETE(myPopupTextService);
	SAFE_DELETE(myDialogueSystem);
	SAFE_DELETE(myPhysXWrapper);
	SAFE_DELETE(myMaterialHandler);
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

CPhysXWrapper& CMainSingleton::PhysXWrapper()
{
	return *ourInstance->myPhysXWrapper;
}

CMaterialHandler& CMainSingleton::MaterialHandler()
{
	return *ourInstance->myMaterialHandler;
}
