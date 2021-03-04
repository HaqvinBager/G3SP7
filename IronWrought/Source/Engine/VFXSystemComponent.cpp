#include "stdafx.h"
#include "VFXSystemComponent.h"

#include "GameObject.h"
#include "CameraComponent.h"

#include "Engine.h"
#include "Scene.h"

#include "Camera.h"
#include "VFXBase.h"
#include "VFXMeshFactory.h"
#include "ParticleEmitterFactory.h"

#include "JsonReader.h"
#include "RandomNumberGenerator.h"

CVFXSystemComponent::CVFXSystemComponent(CGameObject& aParent, const std::string& aVFXDataPath)
	: CBehaviour(aParent) 
{
	myEnabled = true;

	rapidjson::Document doc = CJsonReader::Get()->LoadDocument(aVFXDataPath);
	ENGINE_BOOL_POPUP(!CJsonReader::HasParseError(doc), "Invalid Json document: %s", aVFXDataPath.c_str());
	

	std::vector<std::string> vfxPaths;
	int size = static_cast<int>(doc["VFXMeshes"].Size());
	myVFXAngularSpeeds.resize(size);
	myVFXTransforms.resize(size);
	myVFXTransformsOriginal.resize(size);
	myVFXShouldOrbit.resize(size);
	myVFXDelays.resize(size, 0.0f);
	myVFXDurations.resize(size, 0.0f);
	myVFXBaseDelays.resize(size, 0.0f);
	myVFXBaseDurations.resize(size, 0.0f);
	myVFXIsActive.resize(size, false);
	for (unsigned int i = 0; i < doc["VFXMeshes"].Size(); ++i) {
		vfxPaths.emplace_back(ASSETPATH(doc["VFXMeshes"][i]["Path"].GetString()));

		Matrix t;
		t = Matrix::CreateFromYawPitchRoll
		( DirectX::XMConvertToRadians(doc["VFXMeshes"][i]["Rotation Y"].GetFloat())
		 , DirectX::XMConvertToRadians(doc["VFXMeshes"][i]["Rotation X"].GetFloat())
		 , DirectX::XMConvertToRadians(doc["VFXMeshes"][i]["Rotation Z"].GetFloat())
		);

		t *= Matrix::CreateScale
		 ( doc["VFXMeshes"][i]["Scale X"].GetFloat()
		 , doc["VFXMeshes"][i]["Scale Y"].GetFloat()
		 , doc["VFXMeshes"][i]["Scale Z"].GetFloat()
		);

		t.Translation
		({ doc["VFXMeshes"][i]["Offset X"].GetFloat()
		 , doc["VFXMeshes"][i]["Offset Y"].GetFloat()
		 , doc["VFXMeshes"][i]["Offset Z"].GetFloat() }
		);
		myVFXTransforms[i] = t;
		myVFXTransformsOriginal[i] = t;

		Vector3 r;
		r.x = DirectX::XMConvertToRadians(doc["VFXMeshes"][i]["Rotate X per second"].GetFloat());
		r.y = DirectX::XMConvertToRadians(doc["VFXMeshes"][i]["Rotate Y per second"].GetFloat());
		r.z = DirectX::XMConvertToRadians(doc["VFXMeshes"][i]["Rotate Z per second"].GetFloat());
		myVFXAngularSpeeds[i] = r;

		myVFXShouldOrbit[i] = doc["VFXMeshes"][i]["Orbit"].GetBool();

		myVFXBaseDelays[i] = doc["VFXMeshes"][i]["Delay"].GetFloat();
		myVFXBaseDurations[i] = doc["VFXMeshes"][i]["Duration"].GetFloat();
	}

	myVFXBases = CVFXMeshFactory::GetInstance()->GetVFXBaseSet(vfxPaths);
	ENGINE_BOOL_POPUP(!myVFXBases.empty(), "No VFX data found.");

	std::vector<std::string> particlePaths;
	size = static_cast<int>(doc["ParticleSystems"].Size());
	myEmitterTransforms.resize(size);
	myEmitterTransformsOriginal.resize(size);
	myEmitterAngularSpeeds.resize(size);
	myEmitterShouldOrbit.resize(size);
	myEmitterDelays.resize(size, 0.0f);
	myEmitterDurations.resize(size, 0.0f);
	myEmitterBaseDelays.resize(size);
	myEmitterBaseDurations.resize(size);
	myEmitterTimers.resize(size, 0.0f);
	for (unsigned int i = 0; i < doc["ParticleSystems"].Size(); ++i) {
		particlePaths.emplace_back(ASSETPATH(doc["ParticleSystems"][i]["Path"].GetString()));

		Matrix t;
		t = Matrix::CreateFromYawPitchRoll
		 ( DirectX::XMConvertToRadians(doc["ParticleSystems"][i]["Rotation Y"].GetFloat())
		 , DirectX::XMConvertToRadians(doc["ParticleSystems"][i]["Rotation X"].GetFloat())
		 , DirectX::XMConvertToRadians(doc["ParticleSystems"][i]["Rotation Z"].GetFloat())
		);

		t *= Matrix::CreateScale(doc["ParticleSystems"][i]["Scale"].GetFloat());

		t.Translation
		({ doc["ParticleSystems"][i]["Offset X"].GetFloat()
		 , doc["ParticleSystems"][i]["Offset Y"].GetFloat()
		 , doc["ParticleSystems"][i]["Offset Z"].GetFloat() }
		);

		myEmitterTransforms[i] = t;
		myEmitterTransformsOriginal[i] = t;

		Vector3 r;
		r.x = DirectX::XMConvertToRadians(doc["ParticleSystems"][i]["Rotate X per second"].GetFloat());
		r.y = DirectX::XMConvertToRadians(doc["ParticleSystems"][i]["Rotate Y per second"].GetFloat());
		r.z = DirectX::XMConvertToRadians(doc["ParticleSystems"][i]["Rotate Z per second"].GetFloat());
		myEmitterAngularSpeeds[i] = r;

		myEmitterShouldOrbit[i] = doc["ParticleSystems"][i]["Orbit"].GetBool();

		myEmitterBaseDelays[i] = doc["ParticleSystems"][i]["Delay"].GetFloat();
		myEmitterBaseDurations[i] = doc["ParticleSystems"][i]["Duration"].GetFloat();
	}

	myParticleEmitters = CParticleEmitterFactory::GetInstance()->GetParticleSet(particlePaths);
	ENGINE_BOOL_POPUP(!myParticleEmitters.empty(), "No Particle data found.");
	
	for (unsigned int i = 0; i < myParticleEmitters.size(); ++i) {

		myParticleVertices.emplace_back(std::vector<CParticleEmitter::SParticleVertex>());
		myParticlePools.emplace_back(std::queue<CParticleEmitter::SParticleVertex>());

		myParticleVertices[i].reserve(myParticleEmitters[i]->GetParticleData().myNumberOfParticles);
		for (unsigned int j = 0; j < myParticleEmitters[i]->GetParticleData().myNumberOfParticles; ++j) {
			myParticlePools[i].push(CParticleEmitter::SParticleVertex());
		}
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
			myVFXIsActive[i] = false;
			continue;
		}
		myVFXIsActive[i] = true;
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
		if(myVFXShouldOrbit[i])
			myVFXTransformsOriginal[i] *=  Matrix::CreateFromYawPitchRoll(CTimer::Dt() * myVFXAngularSpeeds[i].y, CTimer::Dt() * myVFXAngularSpeeds[i].x, CTimer::Dt() * myVFXAngularSpeeds[i].z);
		else
		{
			Vector3 t = myVFXTransformsOriginal[i].Translation();
			myVFXTransformsOriginal[i] *=  Matrix::CreateFromYawPitchRoll(CTimer::Dt() * myVFXAngularSpeeds[i].y, CTimer::Dt() * myVFXAngularSpeeds[i].x, CTimer::Dt() * myVFXAngularSpeeds[i].z);
			myVFXTransformsOriginal[i].Translation(t);
		}
		myVFXTransforms[i] = myVFXTransformsOriginal[i] * Matrix::CreateFromQuaternion(quat);

		myVFXTransforms[i].Translation(goPos + goTransform.Right()	 * myVFXTransformsOriginal[i].Translation().x);
		myVFXTransforms[i].Translation(myVFXTransforms[i].Translation() + goTransform.Up()		* myVFXTransformsOriginal[i].Translation().y);
		myVFXTransforms[i].Translation(myVFXTransforms[i].Translation() - goTransform.Forward() * myVFXTransformsOriginal[i].Translation().z);
	}

	for (unsigned int i = 0; i < myEmitterTransforms.size(); ++i)
	{
		if(myEmitterShouldOrbit[i])
			myEmitterTransformsOriginal[i] *=  Matrix::CreateFromYawPitchRoll(CTimer::Dt() * myEmitterAngularSpeeds[i].y, CTimer::Dt() * myEmitterAngularSpeeds[i].x, CTimer::Dt() * myEmitterAngularSpeeds[i].z);
		else
		{
			Vector3 t = myEmitterTransformsOriginal[i].Translation();
			myEmitterTransformsOriginal[i] *=  Matrix::CreateFromYawPitchRoll(CTimer::Dt() * myEmitterAngularSpeeds[i].y, CTimer::Dt() * myEmitterAngularSpeeds[i].x, CTimer::Dt() * myEmitterAngularSpeeds[i].z);
			myEmitterTransformsOriginal[i].Translation(t);
		}

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
		myVFXDelays[i] = myVFXBaseDelays[i];
		myVFXDurations[i] = myVFXBaseDurations[i];
	}

	for (unsigned int i = 0; i < myParticleEmitters.size(); ++i) {
		myEmitterDelays[i] = myEmitterBaseDelays[i];
		myEmitterDurations[i] = myEmitterBaseDurations[i];
	}
}

void CVFXSystemComponent::OnDisable()
{
	Enabled(false);
	for (unsigned int i = 0; i < myVFXBases.size(); ++i) {
		myVFXIsActive[i] = false;
	}

	for (unsigned int i = 0; i < myParticleEmitters.size(); ++i) {
		size_t currentSize = myParticleVertices[i].size();
		for (unsigned int j = 0; j < currentSize; ++j) {
			myParticlePools[i].push(myParticleVertices[i].back());
			myParticleVertices[i].pop_back();
		}
		myEmitterDelays[i] = 0.0f;
		myEmitterDurations[i] = 0.0f;
	}
}

void CVFXSystemComponent::ResetParticles()
{
	for (unsigned int i = 0; i < myParticleEmitters.size(); ++i) {
		myEmitterDelays[i] = myEmitterBaseDelays[i];
		myEmitterDurations[i] = myEmitterBaseDurations[i];
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
		{	  ((Random(someParticleData.myOffsetLowerBound.x, someParticleData.myOffsetUpperBound.x)) * (1.0f))
			, ((Random(someParticleData.myOffsetLowerBound.y, someParticleData.myOffsetUpperBound.y)) * (1.0f))
			, ((Random(someParticleData.myOffsetLowerBound.z, someParticleData.myOffsetUpperBound.z)) * (1.0f))
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