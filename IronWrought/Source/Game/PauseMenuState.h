#pragma once
#include "State.h"
#include "StateStack.h"
#include "Observer.h"
#include "PostMaster.h"

class CPauseMenuState: public CState, public IStringObserver, public IObserver
{
	friend class CScene;
public:
	CPauseMenuState(CStateStack& aStateStack, const CStateStack::EState aState = CStateStack::EState::PauseMenu);
	~CPauseMenuState() override;

	void Awake() override;
	void Start() override;
	void Stop() override;
	void Update() override;

	void Receive(const SStringMessage& aMessage) override;
	void Receive(const SMessage& aMessage) override;
	
};

