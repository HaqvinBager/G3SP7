#pragma once
#include <vector>
#include "GameObject.h"
#include "Component.h"
#include "SpriteInstance.h"
#include "EngineDefines.h"
#include "PhysXWrapper.h"

class CModelComponent;
class CCamera;
class CEnvironmentLight;
class CCollisionManager;
class CPointLight;
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

typedef std::pair<unsigned int, std::array<CPointLight*, LIGHTCOUNT>> LightPair;

class CScene {
	friend class CEngine;
	friend class CInGameState;
	friend class CMenuState;
public:
//SETUP START
	CScene(const unsigned int aGameObjectCount = 0);
	~CScene();

	//static CScene* GetInstance();
	bool Init();
	bool InitNavMesh(std::string aPath);
//SETUP END
public:
//SETTERS START
	void MainCamera(CCameraComponent* aMainCamera);
	bool EnvironmentLight(CEnvironmentLight* anEnvironmentLight);
	void ShouldRenderLineInstance(const bool aShouldRender);
	void UpdateCanvas();
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
	std::pair<unsigned int, std::array<CPointLight*, LIGHTCOUNT>> CullLights(CGameObject* aGameObject);
	const std::vector<CLineInstance*>& CullLineInstances() const;
	const std::vector<SLineTime>& CullLines() const;
	std::vector<CAnimatedUIElement*> CullAnimatedUI(std::vector<CSpriteInstance*>& someFramesToReturn);
	LightPair CullLightInstanced(CInstancedModelComponent* aModelType);
	std::vector<CGameObject*> CullGameObjects(CCameraComponent* aMainCamera);
	std::vector<CSpriteInstance*> CullSprites();
	CGameObject* FindObjectWithID(const int aGameObjectInstanceID);
//CULLING END
public:
	//POPULATE SCENE START
	bool AddInstance(CPointLight* aPointLight);
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
	bool RemoveInstance(CAnimatedUIElement* anAnimatedUIElement);
	bool RemoveInstance(CGameObject* aGameObject);
//REMOVE SPECIFIC INSTANCE END
//CLEAR SCENE OF INSTANCES START
	bool ClearPointLights();
	bool ClearLineInstances();
	bool ClearAnimatedUIElement();
	bool ClearTextInstances();
	bool ClearGameObjects();
	bool ClearSprites();
//CLEAR SCENE OF INSTANCES START

	//Ev Remove // Ev? / Aki & Haqvin
	//std::vector<CPointLight*> LightsNearestPlayer() { return myLightsSortedNearestPlayer;  }
	//This will run every 5-10 frames (It doesn't need to run all the time anyway!)
	//void UpdateLightsNearestPlayer();
	//bool AddEnemies(CGameObject* aEnemy);
	//bool AddBoss(CGameObject* aBoss);
	//bool AddDestructible(CGameObject* aDestructible);
	//bool AddPlayer(CGameObject* aPlayer);
	/*std::vector<CGameObject*> GetEnemies() { return myEnemies; }
	std::vector<CGameObject*> GetDestructibles() { return myDestructibles; }
	CGameObject* GetBoss() { return myBoss; }*/
	//CGameObject* GetPlayer() { return myPlayer; }
	//void SetPlayerToOutline(CGameObject* aPlayer);
	//void SetEnemyToOutline(CGameObject* anEnemy);
	//void TakeOwnershipOfAIBehavior(IAIBehavior* aBehavior);

private:
	//Struct left because it might be needed later
	struct NearestPlayerComparer {
		DirectX::SimpleMath::Vector3 myPos;
		bool operator()(const CPointLight* a, const CPointLight* b) const;
	} ourNearestPlayerComparer;
private:
//CONTAINERS START
	std::vector<CPointLight*> myPointLights;
	std::vector<CLineInstance*> myLineInstances;
	std::vector<CAnimatedUIElement*> myAnimatedUIElements;
	std::vector<CTextInstance*> myTexts;
	std::vector<CGameObject*> myGameObjects;
	std::vector<CGameObject*> myModelsToOutline;
	
	std::unordered_map<int, CGameObject*> myIDGameObjectMap;

	std::unordered_map<ERenderOrder, std::vector<CSpriteInstance*>> mySpriteInstances;
//CONTAINERS END
private:
//POINTERS START
	CEnvironmentLight* myEnvironmentLight;
	SNavMesh* myNavMesh;
	CLineInstance* myNavMeshGrid;
	CCameraComponent* myMainCamera;
	//PhysX scene
	PxScene* myPXScene;

	CCanvas* myCanvas;
//POINTERS END

	bool myIsReadyToRender;

//Ev Remove // Ev? / Aki & Haqvin
//std::vector<CPointLight*> myLightsSortedNearestPlayer;
//std::vector<CCamera*> myCameras;
//std::vector<CEnvironmentLight*> myEnvironmentLights;
//std::vector<CGameObject*> myEnemies;
//std::vector<CGameObject*> myDestructibles;
//CGameObject* myPlayer;
//CGameObject* myBoss;
//IAIBehavior* myEnemyBehavior;
//CCollisionManager* myCollisionManager;
//static CScene* ourInstance;
#ifdef  _DEBUG
private:
	bool myShouldRenderLineInstance;
	CLineInstance* myGrid;
#endif //  _DEBUG
};
