#include "stdafx.h"
#include "VFXSystemComponent.h"

#include "GameObject.h"
#include "CameraComponent.h"

#include "Engine.h"
#include "Scene.h"

#include "Camera.h"
#include "VFXBase.h"
#include "VFXFactory.h"


#include "RandomNumberGenerator.h"

CVFXSystemComponent::CVFXSystemComponent(
	  CGameObject& aParent
	, const std::vector<std::string>& someVFXPaths
	, const std::vector<Matrix>& someTransforms
	, const std::vector<CParticleEmitter*>& someParticles
	, const std::vector<Matrix>& someEmitterTransforms
)
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

	myParticleEmitters = someParticles;
	myEmitterTransforms = someEmitterTransforms;
	myEmitterTransformsOriginal = someEmitterTransforms;

	for (unsigned int i = 0; i < myParticleEmitters.size(); ++i) {

		myParticleVertices.emplace_back(std::vector<CParticleEmitter::SParticleVertex>());
		myParticlePools.emplace_back(std::queue<CParticleEmitter::SParticleVertex>());

		myParticleVertices[i].reserve(myParticleEmitters[i]->GetParticleData().myNumberOfParticles);
		for (unsigned int j = 0; j < myParticleEmitters[i]->GetParticleData().myNumberOfParticles; ++j) {
			myParticlePools[i].push(CParticleEmitter::SParticleVertex());
		}

		myEmitterDelays.emplace_back(myParticleEmitters[i]->GetParticleData().myDelay);
		myEmitterDurations.emplace_back(myParticleEmitters[i]->GetParticleData().myDuration);
		myEmitterTimers.emplace_back(0.0f);
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

	Vector3 cameraPos = IRONWROUGHT_ACTIVE_SCENE.MainCamera()->GameObject().myTransform->Position();

	for (unsigned int i = 0; i < myParticleEmitters.size(); ++i) 
	{
		CParticleEmitter::SParticleData particleData = myParticleEmitters[i]->GetParticleData();

		if ((myEmitterDelays[i] -= CTimer::Dt()) > 0.0f) { continue; }

		Vector3		scale;
		Quaternion	quat;
		Vector3		translation;
		myEmitterTransformsOriginal[i].Decompose(scale, quat, translation);

		if ((myEmitterDurations[i] -= CTimer::Dt()) > 0.0f) {
			SpawnParticles(i, cameraPos, particleData, translation, scale.x);
		}

		UpdateParticles(i, cameraPos, particleData, scale.x);
	}
}

void CVFXSystemComponent::LateUpdate()
{
	if (!Enabled()) return;

	Vector3		scale;
	Quaternion	quat;
	Vector3		translation;
	GameObject().myTransform->GetWorldMatrix().Decompose(scale, quat, translation);

	Matrix goTransform = GameObject().myTransform->Transform();
	Vector3 goPos = GameObject().myTransform->Position();

	for (unsigned int i = 0; i < myVFXTransforms.size(); ++i)
	{
		myVFXTransforms[i] = myVFXTransformsOriginal[i] * Matrix::CreateFromQuaternion(quat);

		myVFXTransforms[i].Translation(goPos + goTransform.Right()	 * myVFXTransformsOriginal[i].Translation().x);
		myVFXTransforms[i].Translation(myVFXTransforms[i].Translation() + goTransform.Up()		* myVFXTransformsOriginal[i].Translation().y);
		myVFXTransforms[i].Translation(myVFXTransforms[i].Translation() - goTransform.Forward() * myVFXTransformsOriginal[i].Translation().z);
	}

	for (unsigned int i = 0; i < myEmitterTransforms.size(); ++i)
	{
		myEmitterTransforms[i] = myEmitterTransformsOriginal[i] * Matrix::CreateFromQuaternion(quat);

		myEmitterTransforms[i].Translation(goPos + goTransform.Right() * myEmitterTransformsOriginal[i].Translation().x);
		myEmitterTransforms[i].Translation(myEmitterTransforms[i].Translation() + goTransform.Up()		* myEmitterTransformsOriginal[i].Translation().y);
		myEmitterTransforms[i].Translation(myEmitterTransforms[i].Translation() - goTransform.Forward() * myEmitterTransformsOriginal[i].Translation().z);
	}
}

void CVFXSystemComponent::OnEnable()
{
	Enabled(true);
	for (unsigned int i = 0; i < myVFXBases.size(); ++i) {
		myVFXDelays[i] = myVFXBases[i]->GetVFXBaseData().myDelay;
		myVFXDurations[i] = myVFXBases[i]->GetVFXBaseData().myDuration;
	}

	for (unsigned int i = 0; i < myParticleEmitters.size(); ++i) {
		myEmitterDelays[i] = myParticleEmitters[i]->GetParticleData().myDelay;
		myEmitterDurations[i] = myParticleEmitters[i]->GetParticleData().myDuration;
	}
}

void CVFXSystemComponent::OnDisable()
{
	Enabled(false);
	for (unsigned int i = 0; i < myVFXBases.size(); ++i) {
		myVFXBases[i]->GetVFXBaseData().myIsActive = false;
	}

	for (unsigned int i = 0; i < myParticleEmitters.size(); ++i) {
		size_t currentSize = myParticleVertices[i].size();
		for (unsigned int j = 0; j < currentSize; ++j) {
			myParticlePools[i].push(myParticleVertices[i].back());
			myParticleVertices[i].pop_back();
		}
	}
}

void CVFXSystemComponent::ResetParticles()
{
	for (unsigned int i = 0; i < myParticleEmitters.size(); ++i) {
		myEmitterDelays[i] = myParticleEmitters[i]->GetParticleData().myDelay;
		myEmitterDurations[i] = myParticleEmitters[i]->GetParticleData().myDuration;
	}
	for (unsigned int i = 0; i < myParticleEmitters.size(); ++i) {
		size_t currentSize = myParticleVertices[i].size();
		for (unsigned int j = 0; j < currentSize; ++j) {
			myParticlePools[i].push(myParticleVertices[i].back());
			myParticleVertices[i].pop_back();
		}
	}
}

void CVFXSystemComponent::SpawnParticles(unsigned int anIndex, DirectX::SimpleMath::Vector3& aCameraPosition, CParticleEmitter::SParticleData& someParticleData, const Vector3& aTranslation, const float aScale)
{
	myEmitterTimers[anIndex] += CTimer::Dt();
	if (myEmitterTimers[anIndex] > (1.0f / someParticleData.mySpawnRate) && (myParticleVertices[anIndex].size() < someParticleData.myNumberOfParticles)) {
		myParticleVertices[anIndex].emplace_back(myParticlePools[anIndex].front());
		myParticlePools[anIndex].pop();
		myParticleVertices[anIndex].back().myLifeTime = someParticleData.myParticleLifetime + Random(someParticleData.myLifetimeLowerBound, someParticleData.myLifetimeUpperBound);
		myParticleVertices[anIndex].back().myPosition = 
		{	  ((someParticleData.myOffsetPosition.x + Random(someParticleData.myOffsetLowerBound.x, someParticleData.myOffsetUpperBound.x)) * (1.0f))
			, ((someParticleData.myOffsetPosition.y + Random(someParticleData.myOffsetLowerBound.y, someParticleData.myOffsetUpperBound.y)) * (1.0f))
			, ((someParticleData.myOffsetPosition.z + Random(someParticleData.myOffsetLowerBound.z, someParticleData.myOffsetUpperBound.z)) * (1.0f))
			, 1.0f 
		};
		myParticleVertices[anIndex].back().myMovement = someParticleData.myParticleStartDirection;
		myParticleVertices[anIndex].back().myStartMovement = someParticleData.myParticleStartDirection + Random(someParticleData.myDirectionLowerBound, someParticleData.myDirectionUpperBound, 0.0f);
		myParticleVertices[anIndex].back().myEndMovement = someParticleData.myParticleEndDirection + Random(someParticleData.myDirectionLowerBound, someParticleData.myDirectionUpperBound, 0.0f);
		myParticleVertices[anIndex].back().myColor = someParticleData.myParticleStartColor;


		myParticleVertices[anIndex].back().mySize = { someParticleData.myParticleStartSize * aScale, someParticleData.myParticleStartSize * aScale };
		myParticleVertices[anIndex].back().mySquaredDistanceToCamera = Vector3::DistanceSquared({ aTranslation.x, aTranslation.y, aTranslation.z }, aCameraPosition);
		myEmitterTimers[anIndex] -= (1.0f / someParticleData.mySpawnRate);
	}
}

void CVFXSystemComponent::UpdateParticles(unsigned int anIndex, DirectX::SimpleMath::Vector3& aCameraPosition, CParticleEmitter::SParticleData& particleData, const float aScale)
{
	std::vector<unsigned int> indicesOfParticlesToRemove;
	for (UINT i = 0; i < myParticleVertices[anIndex].size(); ++i)
	{
		float quotient = myParticleVertices[anIndex][i].myLifeTime / particleData.myParticleLifetime;

		myParticleVertices[anIndex][i].myColor = Vector4::Lerp
		(
			particleData.myParticleEndColor,
			particleData.myParticleStartColor,
			quotient
		);

		myParticleVertices[anIndex][i].mySize = Vector2::Lerp
		(
			{ particleData.myParticleEndSize * aScale, particleData.myParticleEndSize * aScale },
			{ particleData.myParticleStartSize * aScale, particleData.myParticleStartSize * aScale },
			quotient
		);

		myParticleVertices[anIndex][i].myMovement = Vector4::Lerp
		(
			particleData.myParticleEndDirection,
			particleData.myParticleStartDirection,
			quotient
		);

		Vector4 direction = Vector4::Lerp
		(
			myParticleVertices[anIndex][i].myEndMovement,
			myParticleVertices[anIndex][i].myStartMovement,
			quotient
		);

		myParticleVertices[anIndex][i].mySquaredDistanceToCamera = Vector3::DistanceSquared
		(
			{ myParticleVertices[anIndex][i].myPosition.x,
			myParticleVertices[anIndex][i].myPosition.y,
			myParticleVertices[anIndex][i].myPosition.z },
			aCameraPosition
		);

		Vector4 newPosition = direction * particleData.myParticleSpeed * CTimer::Dt() + myParticleVertices[anIndex][i].myPosition;
		myParticleVertices[anIndex][i].myPosition = newPosition;

		if ((myParticleVertices[anIndex][i].myLifeTime -= CTimer::Dt()) < 0.0f) {
			indicesOfParticlesToRemove.emplace_back(i);
		}
	}

	std::sort(indicesOfParticlesToRemove.begin(), indicesOfParticlesToRemove.end(), [](UINT a, UINT b) { return a > b; });
	for (UINT i = 0; i < indicesOfParticlesToRemove.size(); ++i) {
		std::swap(myParticleVertices[anIndex][indicesOfParticlesToRemove[i]], myParticleVertices[anIndex].back());
		myParticlePools[anIndex].push(myParticleVertices[anIndex].back());
		myParticleVertices[anIndex].pop_back();
	}
}