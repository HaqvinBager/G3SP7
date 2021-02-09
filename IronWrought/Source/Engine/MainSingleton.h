#pragma once

class CPostMaster;
class CCollisionManager;
class CDialogueSystem;
class CPopupTextService;
class CMaterialHandler;
class CImguiManager;

class CMainSingleton
{
	friend class CEngine;

public:
	static CPostMaster& PostMaster();
	static CCollisionManager& CollisionManager();
	static CPopupTextService& PopupTextService();
	static CDialogueSystem& DialogueSystem();
	static CMaterialHandler& MaterialHandler();
	static CImguiManager& ImguiManager();

private:
	static CMainSingleton* ourInstance;

	CPostMaster*			myPostMaster;
	CCollisionManager*		myCollisionManager;
	CPopupTextService*		myPopupTextService;
	CDialogueSystem*		myDialogueSystem;
	CMaterialHandler*		myMaterialHandler;
	CImguiManager*			myImguiManager;

private:
	CMainSingleton();
	~CMainSingleton();
};

