#include "stdafx.h"
#include "InGameState.h"

#include <GameObject.h>
#include <TransformComponent.h>
#include <ModelComponent.h>
#include <DecalComponent.h>
#include <PointLightComponent.h>

#include <Scene.h>
#include <Engine.h>
#include <PostMaster.h>
#include <MainSingleton.h>
#include <CollisionManager.h>
#include <PointLight.h>

#include <JsonReader.h>
#include <SceneManager.h>

#ifndef NDEBUG
	#include <VFXSystemComponent.h>
	#include <VFXMeshFactory.h>
	#include <ParticleEmitterFactory.h>
	
	#include <TextFactory.h>
	#include <TextInstance.h>
	#include "EnemyComponent.h"
	
	void TEMP_VFX(CScene* aScene);
#endif

CInGameState::CInGameState(CStateStack& aStateStack, const CStateStack::EState aState)
	: CState(aStateStack, aState),
	myExitLevel(false)
{
}

CInGameState::~CInGameState() {}


void CInGameState::Awake()
{
	CJsonReader::Get()->InitFromGenerated();
	CScene* scene = CSceneManager::CreateEmpty();
#ifndef NDEBUG
	TEMP_VFX(scene);
#endif
	CEngine::GetInstance()->AddScene(myState, scene);
	CMainSingleton::PostMaster().Subscribe("LoadScene", this);
}


void CInGameState::Start()
{
	CEngine::GetInstance()->SetActiveScene(myState);
	IRONWROUGHT->GetActiveScene().CanvasIsHUD();
	//TEMP_VFX(&CEngine::GetInstance()->GetActiveScene());

	myExitLevel = false;
}

void CInGameState::Stop()
{
	IRONWROUGHT->RemoveScene(myState);
	CMainSingleton::CollisionManager().ClearColliders();
}

void CInGameState::Update()
{

	IRONWROUGHT->GetActiveScene().UpdateCanvas();

	if (Input::GetInstance()->IsKeyPressed(VK_ESCAPE))
	{
		myStateStack.PushState(CStateStack::EState::PauseMenu);
	}

}

void CInGameState::ReceiveEvent(const EInputEvent aEvent)
{
	if (this == myStateStack.GetTop())
	{
		switch (aEvent)
		{
			case IInputObserver::EInputEvent::PauseGame:
				//myStateStack.PushState(CStateStack::EState::PauseMenu);
				break;
			default:
				break;
		}
	}
}

void CInGameState::Receive(const SStringMessage& aMessage)
{
	const char* test = "LoadScene";
	if (aMessage.myMessageType == test)
	{
		Start();
	}
	myExitLevel = true;
}

void CInGameState::Receive(const SMessage& /*aMessage*/)
{
	//switch (aMessage.myMessageType)
	//{
	//	default:break;
	//}
}

#ifndef NDEBUG
	void TEMP_VFX(CScene* aScene)
	{
		static int id = 500;
		CGameObject* abilityObject = new CGameObject(id++);
		abilityObject->AddComponent<CVFXSystemComponent>(*abilityObject, ASSETPATH("Assets/Graphics/VFX/JSON/VFXSystem_ToLoad.json"));
	
		aScene->AddInstance(abilityObject);
		aScene->SetVFXTester(abilityObject);
	}
#endif
