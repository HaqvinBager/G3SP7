#pragma once
#include <vector>
#include "GameObject.h"
#include "Component.h"
#include "SpriteInstance.h"
#include "EngineDefines.h"
#include "PhysXWrapper.h"
#include "PostMaster.h"

class CModelComponent;
class CCamera;
class CEnvironmentLight;
class CCollisionManager;
class CPointLight;
class CSpotLight;
class CBoxLight;
class CLineInstance;
class CCanvas;

class CAnimatedUIElement;
class CTextInstance;
class CCameraComponent;
struct SLineTime;
struct SNavMesh;
class CModel;
class IAIBehavior;
class CInstancedModelComponent;

class CPlayerControllerComponent;

typedef std::pair<unsigned int, std::array<CPointLight*, LIGHTCOUNT>> LightPair;

class CScene : public IObserver {
	friend class CEngine;
	friend class CBootUpState;
	friend class CInGameState;
	friend class CMainMenuState;
	friend class CPauseMenuState;
public:
//SETUP START
	CScene(const unsigned int aGameObjectCount = 0);
	~CScene();

	//static CScene* GetInstance();
	bool Init();
	bool InitNavMesh(const std::string& aPath);
	bool InitCanvas(const std::string& aPath);
	bool ReInitCanvas(const std::string& aPath);
	void CanvasIsHUD();
	/*void UpdateCanvas()*/;
	void DisableWidgetsOnCanvas();
	void CanvasToggle(const bool& anIsEnabled, const bool& anIsForceEnable = false);

//SETUP END

//UPDATE
public:
	void Awake();
	void Start();
	void Update();
	void FixedUpdate();
	void Receive(const SMessage& aMessage) override;

private:
	void InitAnyNewComponents();
	void CallStartOnNewComponents();
	void CallAwakeOnNewComponents();
//UPDATE END

public:
//SETTERS START
	void MainCamera(CCameraComponent* aMainCamera);
	void Player(CGameObject* aPlayerObject);
	bool EnvironmentLight(CEnvironmentLight* anEnvironmentLight);
	void ShouldRenderLineInstance(const bool aShouldRender);
#ifdef _DEBUG
	bool ShouldRenderLineInstance() { return myShouldRenderLineInstance; }
#endif
	
//SETTERS END
public:
	template <class T>
	T* FindFirstObjectWithComponent() {
		for (auto& gameObject : myGameObjects) {
			if (gameObject->GetComponent<T>() != nullptr) {
				return gameObject->GetComponent<T>();
			}
		}
		return nullptr;
	}
public:
//GETTERS START
	CCameraComponent* MainCamera();
	CGameObject* Player();
	CPlayerControllerComponent* PlayerController();
	CEnvironmentLight* EnvironmentLight();
	SNavMesh* NavMesh();
	PxScene* PXScene();
	std::vector<CGameObject*> ModelsToOutline() const;
	std::vector<CPointLight*>& PointLights();
	std::vector<CTextInstance*> Texts();
	const std::vector<CGameObject*>& ActiveGameObjects() const;
//GETTERS END
public:
	const bool Ready() const { return myIsReadyToRender; }
	void Ready(bool aReady) { myIsReadyToRender = aReady; }
public:
//CULLING START
	std::vector<CPointLight*> CullPointLights(CGameObject* aGameObject);
	std::vector<CSpotLight*> CullSpotLights(CGameObject* aGameObject);
	std::vector<CBoxLight*> CullBoxLights(CGameObject* aGameObject);
	std::pair<unsigned int, std::array<CPointLight*, LIGHTCOUNT>> CullLights(CGameObject* aGameObject);
	const std::vector<CLineInstance*>& CullLineInstances() const;
	const std::vector<SLineTime>& CullLines() const;
	std::vector<CAnimatedUIElement*> CullAnimatedUI(std::vector<CSpriteInstance*>& someFramesToReturn);
	LightPair CullLightInstanced(CInstancedModelComponent* aModelType);
	std::vector<CGameObject*> CullGameObjects(CCameraComponent* aMainCamera);
	std::vector<CSpriteInstance*> CullSprites();
	CGameObject* FindObjectWithID(const int aGameObjectInstanceID);
	template<class T>
	std::vector<CComponent*>* GetAllComponents();
//CULLING END
public:
	//POPULATE SCENE START
	bool AddInstance(CPointLight* aPointLight);
	bool AddInstance(CSpotLight* aSpotLight);
	bool AddInstance(CBoxLight* aBoxLight);
	bool AddInstance(CLineInstance* aLineInstance);
	bool AddInstance(CAnimatedUIElement* anAnimatedUIElement);
	bool AddInstance(CTextInstance* aText);
	bool AddInstance(CGameObject* aGameObject);
	bool AddInstances(std::vector<CGameObject*>& someGameObjects);
	bool AddInstance(CSpriteInstance* aSprite);
	//PhysX
	bool AddPXScene(PxScene* aPXScene);
	//POPULATE SCENE END
public:
//REMOVE SPECIFIC INSTANCE START
	bool RemoveInstance(CPointLight* aPointLight);
	bool RemoveInstance(CSpotLight* aSpotLight);
	bool RemoveInstance(CBoxLight* aBoxLight);
	bool RemoveInstance(CAnimatedUIElement* anAnimatedUIElement);
	bool RemoveInstance(CGameObject* aGameObject);
	bool RemoveInstance(CSpriteInstance* aSpriteInstance);
	bool RemoveInstance(CTextInstance* aTextInstance);
//REMOVE SPECIFIC INSTANCE END
//CLEAR SCENE OF INSTANCES START
	bool ClearPointLights();
	bool ClearSpotLights();
	bool ClearBoxLights();
	bool ClearLineInstances();
	bool ClearAnimatedUIElement();
	bool ClearTextInstances();
	bool ClearGameObjects();
	bool ClearSprites();
//CLEAR SCENE OF INSTANCES START


private:
	//Struct left because it might be needed later
	struct NearestPlayerComparer {
		DirectX::SimpleMath::Vector3 myPos;
		bool operator()(const CPointLight* a, const CPointLight* b) const;
	} ourNearestPlayerComparer;
private:
//CONTAINERS START
	std::vector<CPointLight*> myPointLights;
	std::vector<CSpotLight*> mySpotLights;
	std::vector<CBoxLight*> myBoxLights;
	std::vector<CLineInstance*> myLineInstances;
	std::vector<CAnimatedUIElement*> myAnimatedUIElements;
	std::vector<CTextInstance*> myTexts;
	std::vector<CGameObject*> myGameObjects;
	std::vector<CGameObject*> myModelsToOutline;
	std::unordered_map<int, CGameObject*> myIDGameObjectMap;
	std::unordered_map<size_t, std::vector<CComponent*>> myComponentMap;
	std::unordered_map<ERenderOrder, std::vector<CSpriteInstance*>> mySpriteInstances;

	std::queue<CComponent*> myAwakeComponents;
	std::queue<CComponent*> myStartComponents;


//CONTAINERS END
private:
//POINTERS START
	CEnvironmentLight* myEnvironmentLight;
	SNavMesh* myNavMesh;
	CLineInstance* myNavMeshGrid;
	CCameraComponent* myMainCamera;
	PxScene* myPXScene;
	CCanvas* myCanvas;
	CGameObject* myPlayer;
//POINTERS END

// VFX EDITOR START
public:
	void SetVFXTester(CGameObject* aGameObject)
	{
		myVFXTester = aGameObject;
	}
	CGameObject* GetVFXTester() const
	{
		return myVFXTester;
	}
private:
	CGameObject* myVFXTester = nullptr;
// VFX EDITOR END

	bool myIsReadyToRender;
#ifdef  _DEBUG
private:
	bool myShouldRenderLineInstance;
	CLineInstance* myGrid;
#endif //  _DEBUG
};

template<class T>
inline std::vector<CComponent*>* CScene::GetAllComponents()
{
	size_t hashCode = typeid(T).hash_code();
	if (myComponentMap.find(hashCode) == myComponentMap.end())
		return nullptr;
	return &myComponentMap[hashCode];
}
