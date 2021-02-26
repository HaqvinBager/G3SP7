#include "stdafx.h"
#include "VFXSystemComponent.h"

#include "GameObject.h"
#include "VFXBase.h"
#include "VFXFactory.h"

CVFXSystemComponent::CVFXSystemComponent(CGameObject& aParent, const std::vector<std::string>& someVFXPaths)
	: CBehaviour(aParent) 
{
	myEnabled = true;

	myVFXBases = CVFXFactory::GetInstance()->GetVFXBaseSet(someVFXPaths);
	ENGINE_BOOL_POPUP(!myVFXBases.empty(), "No VFX data found.");

	myVFXDelays.reserve(myVFXBases.size());
	myVFXDurations.reserve(myVFXBases.size());
	for (unsigned int i = 0; i < myVFXBases.size(); ++i) {
		myVFXDelays.emplace_back(myVFXBases[i]->GetVFXBaseData().myDelay);
		myVFXDurations.emplace_back(myVFXBases[i]->GetVFXBaseData().myDuration);
	}
}

CVFXSystemComponent::~CVFXSystemComponent()
{
	myVFXBases.clear();
}

void CVFXSystemComponent::Awake()
{}

void CVFXSystemComponent::Start()
{}

void CVFXSystemComponent::Update()
{
	if (!Enabled()) return;

	for (unsigned int i = 0; i < myVFXBases.size(); ++i)
	{
		if ((myVFXDelays[i] -= CTimer::Dt()) > 0.0f) continue;

		if ((myVFXDurations[i] -= CTimer::Dt()) < 0.0f) {
			myVFXBases[i]->GetVFXBaseData().myIsActive = false;
			continue;
		}

		myVFXBases[i]->GetVFXBaseData().myIsActive = true;
	}
}

void CVFXSystemComponent::OnEnable()
{
	Enabled(true);
	for (unsigned int i = 0; i < myVFXBases.size(); ++i) {
		myVFXDelays[i] = myVFXBases[i]->GetVFXBaseData().myDelay;
		myVFXDurations[i] = myVFXBases[i]->GetVFXBaseData().myDuration;
	}
}

void CVFXSystemComponent::OnDisable()
{
	Enabled(false);
	for (unsigned int i = 0; i < myVFXBases.size(); ++i) {
		myVFXBases[i]->GetVFXBaseData().myIsActive = false;
	}
}

bool CVFXSystemComponent::Init()
{



	return true;
}
