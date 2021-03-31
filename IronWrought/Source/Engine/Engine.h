#pragma once
#include "WindowHandler.h"
#include "DirectXFramework.h"

#include "StateStack.h"

#include <string>
#include <vector>
#include <unordered_map>

#define IRONWROUGHT CEngine::GetInstance()
#define IRONWROUGHT_ACTIVE_SCENE CEngine::GetInstance()->GetActiveScene()

class CWindowHandler;
class CDirextXFramework;
class CTimer;
class CModelFactory;
class CCameraFactory;
class CLightFactory;
class CScene;
class CRenderManager;
class CParticleEmitterFactory;
class CSpriteFactory;
class CTextFactory;
class CDecalFactory;
class CInputMapper;
class CDebug;
class CMainSingleton;
class CForwardRenderer;
class CVFXMeshFactory;
class CLineFactory;
class CAudioManager;
class CPhysXWrapper;
class CSceneFactory;
class CGraphManager;

class CEngine
{
	friend class CForwardRenderer;
	friend class CModelFactory;
	friend class CVFXMeshFactory;
	friend class CLightFactory;
	friend class CRenderManager;
	friend class CLineFactory;
	friend class CDecalFactory;
	friend class CMaterialHandler;
	friend class CSceneFactory;

public:
	CEngine();
	~CEngine();
	bool Init(CWindowHandler::SWindowData& someWindowData);
	float BeginFrame();
	void Update();
	void RenderFrame();
	void EndFrame();
	CWindowHandler* GetWindowHandler();
	void InitWindowsImaging();
	void CrashWithScreenShot(std::wstring& aSubPath);

	void SetResolution(DirectX::SimpleMath::Vector2 aResolution);

	static CEngine* GetInstance();
	
	const CStateStack::EState AddScene(const CStateStack::EState aState, CScene* aScene);
	void SetActiveScene(const CStateStack::EState aState);
	CScene& GetActiveScene();
	
	CPhysXWrapper& GetPhysx() { return *myPhysxWrapper; }

	void ModelViewerSetScene(CScene* aScene);
	//void PopBackScene();
	//void SetActiveScene(CScene* aScene);
	
	//unsigned int ScenesSize();

	void SetRenderScene(const bool aRenderSceneActive) { myRenderSceneActive = aRenderSceneActive; }
	void RemoveScene(CStateStack::EState aState);
	void ClearModelFactory();

	void LoadGraph(const std::string& aSceneName);

private:
	static CEngine* ourInstance;
	
	CWindowHandler* myWindowHandler;
	CDirectXFramework* myFramework;
	CForwardRenderer* myForwardRenderer;
	CRenderManager* myRenderManager;
	CTimer* myTimer;
	CDebug* myDebug;
	CPhysXWrapper* myPhysxWrapper;

	//unsigned int myActiveScene;
	CStateStack::EState myActiveState;
	//std::vector<CScene*> myScenes;
	std::unordered_map<CStateStack::EState, CScene*> mySceneMap;

	CModelFactory* myModelFactory;
	CCameraFactory* myCameraFactory;
	CLightFactory* myLightFactory;
	CParticleEmitterFactory* myParticleFactory;
	CVFXMeshFactory* myVFXFactory;
	CLineFactory* myLineFactory;
	CSpriteFactory* mySpriteFactory;
	CTextFactory* myTextFactory;
	CDecalFactory* myDecalFactory;
	CInputMapper* myInputMapper;
	CMainSingleton* myMainSingleton;
	CAudioManager* myAudioManager;
	CSceneFactory* mySceneFactory;
	CGraphManager* myGraphManager;

	bool myRenderSceneActive = false;
};