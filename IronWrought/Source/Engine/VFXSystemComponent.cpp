#include "stdafx.h"
#include "VFXSystemComponent.h"

#include "GameObject.h"
#include "VFXBase.h"
#include "VFXFactory.h"

CVFXSystemComponent::CVFXSystemComponent(CGameObject& aParent, const std::vector<std::string>& someVFXPaths, const std::vector<Matrix>& someTransforms)
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
	
	myVFXTransforms = someTransforms;
	myVFXTransformsOriginal = someTransforms;
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

void CVFXSystemComponent::LateUpdate()
{
	if (!Enabled()) return;

	DirectX::SimpleMath::Vector3 scale;
	DirectX::SimpleMath::Quaternion quat;
	DirectX::SimpleMath::Vector3 translation;
	GameObject().myTransform->GetWorldMatrix().Decompose(scale, quat, translation);

	auto t = myVFXTransformsOriginal[0].Translation();
	myVFXTransformsOriginal[0] = myVFXTransformsOriginal[0] * Matrix::CreateFromYawPitchRoll(fabs(sinf(CTimer::Time()))* CTimer::Dt() * 2.0f, 0.0f, 0.f);
	myVFXTransformsOriginal[0].Translation(t);

	t = myVFXTransformsOriginal[1].Translation();
	myVFXTransformsOriginal[1] = myVFXTransformsOriginal[1] * Matrix::CreateFromYawPitchRoll(0.0f, fabs(sinf(CTimer::Time()))* CTimer::Dt() * 2.0f, 0.f);
	myVFXTransformsOriginal[1].Translation(t);

	t = myVFXTransformsOriginal[2].Translation();
	myVFXTransformsOriginal[2] = myVFXTransformsOriginal[2] * Matrix::CreateFromYawPitchRoll(0.0f, 0.0f, fabs(sinf(CTimer::Time()))* CTimer::Dt() * 2.0f);
	myVFXTransformsOriginal[2].Translation(t);

	Matrix goTransform = GameObject().myTransform->Transform();
	auto goPos = GameObject().myTransform->Position();
	for (unsigned int i = 0; i < myVFXTransforms.size(); ++i)
	{
		myVFXTransforms[i] = myVFXTransformsOriginal[i] * Matrix::CreateFromQuaternion(quat);
		//myVFXTransforms[i].Translation(GameObject().myTransform->Position() + (myVFXTransformsOriginal[i].Translation() ));

		myVFXTransforms[i].Translation(goPos + goTransform.Right()	 * myVFXTransformsOriginal[i].Translation().x);
		myVFXTransforms[i].Translation(myVFXTransforms[i].Translation() + goTransform.Up()		 * myVFXTransformsOriginal[i].Translation().y);
		myVFXTransforms[i].Translation(myVFXTransforms[i].Translation() - goTransform.Forward() * myVFXTransformsOriginal[i].Translation().z);

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
