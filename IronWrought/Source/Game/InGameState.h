#pragma once
#include "State.h"
#include "StateStack.h"
#include "InputObserver.h"
#include "Observer.h"
#include "PostMaster.h"

class CGameObject;
class CMouseSelection;

class CInGameState : public CState, public IInputObserver, public IStringObserver, public IObserver
{
public:
	CInGameState(CStateStack& aStateStack, const CStateStack::EState aState = CStateStack::EState::InGame);
	~CInGameState() override;

	void Awake() override;
	void Start() override;
	void Stop() override;
	void Update() override;

	void ReceiveEvent(const EInputEvent aEvent) override;
	void Receive(const SStringMessage& aMessage) override;
	void Receive(const SMessage& aMessage) override;

	void TEMP_DecalTests(class CScene* aScene);
private:
	enum class EExitTo
	{
		None,
		MainMenu,
		AnotherLevel
	};

	void DEBUGFunctionality();

private:
	bool myExitLevel;
	EExitTo myExitTo;

	class CEnemyAnimationController* myEnemyAnimationController;
	CGameObject* myDecal;
};