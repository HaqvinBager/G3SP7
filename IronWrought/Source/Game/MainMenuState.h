#pragma once
#include "State.h"
#include "StateStack.h"
#include "Observer.h"
#include "PostMaster.h"

class CMainMenuState: public CState, public IStringObserver, public IObserver
{
	friend class CScene;
public:
	CMainMenuState(CStateStack& aStateStack, const CStateStack::EState aState = CStateStack::EState::MainMenu);
	~CMainMenuState() override;

	void Awake() override;
	void Start() override;
	void Stop() override;
	void Update() override;

	void Receive(const SStringMessage& aMessage) override;
	void Receive(const SMessage& aMessage) override;

private:
	// More or less temp. / Aki 2021 04 06
	bool myTimeToQuit;
};

