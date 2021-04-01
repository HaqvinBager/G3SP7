#pragma once
#include "State.h"
#include "StateStack.h"
#include "Observer.h"
#include "PostMaster.h"

class CSpriteInstance;

class CBootUpState : public CState, public IStringObserver, public IObserver
{
public:
	CBootUpState(CStateStack& aStateStack, const CStateStack::EState aState = CStateStack::EState::BootUp);
	~CBootUpState() override;

	void Awake() override;
	void Start() override;
	void Stop() override;
	void Update() override;

	void Receive(const SStringMessage& aMessage) override;
	void Receive(const SMessage& aMessage) override;

private:
	std::vector<CSpriteInstance*> myLogos;
	float myTimer;
	float myLogoDisplayDuration;
	float myFadeOutStart;
	int myLogoToRender;
};

