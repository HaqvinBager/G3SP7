#pragma once
#include "State.h"
#include "StateStack.h"
#include "Observer.h"
#include "PostMaster.h"

class CLoadLevelState: public CState, public IStringObserver, public IObserver
{
	friend class CScene;
public:
	CLoadLevelState(CStateStack& aStateStack, const CStateStack::EState aState = CStateStack::EState::LoadLevel);
	~CLoadLevelState() override;

	void Awake() override;
	void Start() override;
	void Stop() override;
	void Update() override;

	void Receive(const SStringMessage& aMessage) override;
	void Receive(const SMessage& aMessage) override;

	void OnComplete(std::string aSceneThatHasBeenSuccessfullyLoaded);

private:
	std::string myLevelToLoad;
};

