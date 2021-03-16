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

#include "VFXEffect.h"

CVFXSystemComponent::CVFXSystemComponent(CGameObject& aParent, const std::string& aVFXDataPath)
	: CBehaviour(aParent) 
{
	myEnabled = true;

	rapidjson::Document document = CJsonReader::Get()->LoadDocument(aVFXDataPath);
	ENGINE_BOOL_POPUP(!CJsonReader::HasParseError(document), "Invalid Json document: %s", aVFXDataPath.c_str());
	ENGINE_BOOL_POPUP(document.HasMember("VFXEffects"), "VFX Json: %s is not structured correctly! Please compare to VFXSystem_ToLoad.json", aVFXDataPath.c_str());

	for (unsigned int j = 0; j < document["VFXEffects"].Size(); ++j)
	{
		myEffects.push_back(new SVFXEffect());
		auto doc = document["VFXEffects"][j].GetObjectW();
		auto effect = myEffects.back();

		ENGINE_BOOL_POPUP(doc.HasMember("Name"), "VFX Json: %s is not structured correctly! Please compare to VFXSystem_ToLoad.json", aVFXDataPath.c_str());
		effect->myName = doc["Name"].GetString();

		std::vector<std::string> vfxPaths;
		ENGINE_BOOL_POPUP(doc.HasMember("VFXMeshes"), "VFX Json: %s is not structured correctly! Please compare to VFXSystem_ToLoad.json", aVFXDataPath.c_str());
		int size = static_cast<int>(doc["VFXMeshes"].Size());
		effect->myVFXAngularSpeeds.resize(size);
		effect->myVFXTransforms.resize(size);
		effect->myVFXTransformsOriginal.resize(size);
		effect->myVFXShouldOrbit.resize(size);
		effect->myVFXDelays.resize(size, 0.0f);
		effect->myVFXDurations.resize(size, 0.0f);
		effect->myVFXBaseDelays.resize(size, 0.0f);
		effect->myVFXBaseDurations.resize(size, 0.0f);
		effect->myVFXIsActive.resize(size, false);
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
			effect->myVFXTransforms[i] = t;
			effect->myVFXTransformsOriginal[i] = t;

			Vector3 r;
			r.x = DirectX::XMConvertToRadians(doc["VFXMeshes"][i]["Rotate X per second"].GetFloat());
			r.y = DirectX::XMConvertToRadians(doc["VFXMeshes"][i]["Rotate Y per second"].GetFloat());
			r.z = DirectX::XMConvertToRadians(doc["VFXMeshes"][i]["Rotate Z per second"].GetFloat());
			effect->myVFXAngularSpeeds[i] = r;

			effect->myVFXShouldOrbit[i] = doc["VFXMeshes"][i]["Orbit"].GetBool();

			effect->myVFXBaseDelays[i] = doc["VFXMeshes"][i]["Delay"].GetFloat();
			effect->myVFXBaseDurations[i] = doc["VFXMeshes"][i]["Duration"].GetFloat();
		}

		effect->myVFXBases = CVFXMeshFactory::GetInstance()->GetVFXBaseSet(vfxPaths);
		ENGINE_BOOL_POPUP(!effect->myVFXBases.empty(), "No VFX data found.");

		std::vector<std::string> particlePaths;
		ENGINE_BOOL_POPUP(doc.HasMember("ParticleSystems"), "VFX Json: %s is not structured correctly! Please compare to VFXSystem_ToLoad.json", aVFXDataPath.c_str());
		size = static_cast<int>(doc["ParticleSystems"].Size());
		effect->myEmitterTransforms.resize(size);
		effect->myEmitterTransformsOriginal.resize(size);
		effect->myEmitterAngularSpeeds.resize(size);
		effect->myEmitterShouldOrbit.resize(size);
		effect->myEmitterDelays.resize(size, 0.0f);
		effect->myEmitterDurations.resize(size, 0.0f);
		effect->myEmitterBaseDelays.resize(size);
		effect->myEmitterBaseDurations.resize(size);
		effect->myEmitterTimers.resize(size, 0.0f);
		effect->myParticleSizeCurves.resize(size);
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

			effect->myEmitterTransforms[i] = t;
			effect->myEmitterTransformsOriginal[i] = t;

			Vector3 r;
			r.x = DirectX::XMConvertToRadians(doc["ParticleSystems"][i]["Rotate X per second"].GetFloat());
			r.y = DirectX::XMConvertToRadians(doc["ParticleSystems"][i]["Rotate Y per second"].GetFloat());
			r.z = DirectX::XMConvertToRadians(doc["ParticleSystems"][i]["Rotate Z per second"].GetFloat());
			effect->myEmitterAngularSpeeds[i] = r;

			effect->myEmitterShouldOrbit[i] = doc["ParticleSystems"][i]["Orbit"].GetBool();

			effect->myEmitterBaseDelays[i] = doc["ParticleSystems"][i]["Delay"].GetFloat();
			effect->myEmitterBaseDurations[i] = doc["ParticleSystems"][i]["Duration"].GetFloat();

			if (doc["ParticleSystems"][i].HasMember("SizeCurve"))
			{
				const auto& sizeCurveArray = doc["ParticleSystems"][i]["SizeCurve"].GetArray();
				unsigned int curveIndex = 0;
				for (const auto& point : sizeCurveArray)
				{
					effect->myParticleSizeCurves[i][curveIndex].x = point["x"].GetFloat();
					effect->myParticleSizeCurves[i][curveIndex].y = point["y"].GetFloat();
					curveIndex++;
				}
			}
		}

		effect->myParticleEmitters = CParticleEmitterFactory::GetInstance()->GetParticleSet(particlePaths);
		ENGINE_BOOL_POPUP(!effect->myParticleEmitters.empty(), "No Particle data found.");
	
		for (unsigned int i = 0; i < effect->myParticleEmitters.size(); ++i) {

			effect->myParticleVertices.emplace_back(std::vector<CParticleEmitter::SParticleVertex>());
			effect->myParticlePools.emplace_back(std::queue<CParticleEmitter::SParticleVertex>());

			effect->myParticleVertices[i].reserve(effect->myParticleEmitters[i]->GetParticleData().myNumberOfParticles);
			for (unsigned int k = 0; k < effect->myParticleEmitters[i]->GetParticleData().myNumberOfParticles; ++k) {
				effect->myParticlePools[i].push(CParticleEmitter::SParticleVertex());
			}
		}
	}
}

CVFXSystemComponent::~CVFXSystemComponent()
{
	myEffects.clear();
}

void CVFXSystemComponent::Awake()
{}

void CVFXSystemComponent::Start()
{}

void CVFXSystemComponent::Update()
{
	if (!Enabled()) return;

	for (unsigned int j = 0; j < myEffects.size(); ++j)
	{
		auto effect = myEffects[j];

		if (!effect->myIsEnabled) continue;

		for (unsigned int i = 0; i < effect->myVFXBases.size(); ++i)
		{
			if ((effect->myVFXDelays[i] -= CTimer::Dt()) > 0.0f) continue;

			if ((effect->myVFXDurations[i] -= CTimer::Dt()) < 0.0f) {
				effect->myVFXIsActive[i] = false;
				continue;
			}
			effect->myVFXIsActive[i] = true;
		}

		Vector3 cameraPos = IRONWROUGHT_ACTIVE_SCENE.MainCamera()->GameObject().myTransform->Position();

		for (unsigned int i = 0; i < effect->myParticleEmitters.size(); ++i)
		{
			CParticleEmitter::SParticleData particleData = effect->myParticleEmitters[i]->GetParticleData();

			if ((effect->myEmitterDelays[i] -= CTimer::Dt()) > 0.0f) { continue; }

			Vector3		scale;
			Quaternion	quat;
			Vector3		translation;
			effect->myEmitterTransformsOriginal[i].Decompose(scale, quat, translation);

			if ((effect->myEmitterDurations[i] -= CTimer::Dt()) > 0.0f) {
				effect->SpawnParticles(i, cameraPos, particleData, translation, scale.x);
			}

			effect->UpdateParticles(i, cameraPos, particleData, scale.x);
		}
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

	for (unsigned int j = 0; j < myEffects.size(); ++j)
	{
		auto effect = myEffects[j];

		for (unsigned int i = 0; i < effect->myVFXTransforms.size(); ++i)
		{
			if(effect->myVFXShouldOrbit[i])
				effect->myVFXTransformsOriginal[i] *=  Matrix::CreateFromYawPitchRoll(CTimer::Dt() * effect->myVFXAngularSpeeds[i].y, CTimer::Dt() * effect->myVFXAngularSpeeds[i].x, CTimer::Dt() * effect->myVFXAngularSpeeds[i].z);
			else
			{
				Vector3 t = effect->myVFXTransformsOriginal[i].Translation();
				effect->myVFXTransformsOriginal[i] *=  Matrix::CreateFromYawPitchRoll(CTimer::Dt() * effect->myVFXAngularSpeeds[i].y, CTimer::Dt() * effect->myVFXAngularSpeeds[i].x, CTimer::Dt() * effect->myVFXAngularSpeeds[i].z);
				effect->myVFXTransformsOriginal[i].Translation(t);
			}
			effect->myVFXTransforms[i] = effect->myVFXTransformsOriginal[i] * Matrix::CreateFromQuaternion(quat);

			effect->myVFXTransforms[i].Translation(goPos + goTransform.Right()	 * effect->myVFXTransformsOriginal[i].Translation().x);
			effect->myVFXTransforms[i].Translation(effect->myVFXTransforms[i].Translation() + goTransform.Up()		* effect->myVFXTransformsOriginal[i].Translation().y);
			effect->myVFXTransforms[i].Translation(effect->myVFXTransforms[i].Translation() - goTransform.Forward() * effect->myVFXTransformsOriginal[i].Translation().z);
		}

		for (unsigned int i = 0; i < effect->myEmitterTransforms.size(); ++i)
		{
			if(effect->myEmitterShouldOrbit[i])
				effect->myEmitterTransformsOriginal[i] *=  Matrix::CreateFromYawPitchRoll(CTimer::Dt() * effect->myEmitterAngularSpeeds[i].y, CTimer::Dt() * effect->myEmitterAngularSpeeds[i].x, CTimer::Dt() * effect->myEmitterAngularSpeeds[i].z);
			else
			{
				Vector3 t = effect->myEmitterTransformsOriginal[i].Translation();
				effect->myEmitterTransformsOriginal[i] *=  Matrix::CreateFromYawPitchRoll(CTimer::Dt() * effect->myEmitterAngularSpeeds[i].y, CTimer::Dt() * effect->myEmitterAngularSpeeds[i].x, CTimer::Dt() * effect->myEmitterAngularSpeeds[i].z);
				effect->myEmitterTransformsOriginal[i].Translation(t);
			}

			effect->myEmitterTransforms[i] = effect->myEmitterTransformsOriginal[i] * Matrix::CreateFromQuaternion(quat);

			effect->myEmitterTransforms[i].Translation(goPos + goTransform.Right() * effect->myEmitterTransformsOriginal[i].Translation().x);
			effect->myEmitterTransforms[i].Translation(effect->myEmitterTransforms[i].Translation() + goTransform.Up()		* effect->myEmitterTransformsOriginal[i].Translation().y);
			effect->myEmitterTransforms[i].Translation(effect->myEmitterTransforms[i].Translation() - goTransform.Forward() * effect->myEmitterTransformsOriginal[i].Translation().z);
		}
	}
}

void CVFXSystemComponent::OnEnable()
{
	Enabled(true);

	for (auto effect : myEffects)
	{
		effect->Enable();
	}
}

void CVFXSystemComponent::OnDisable()
{
	Enabled(false);

	for (auto effect : myEffects)
	{
		effect->Disable();
	}
}

void CVFXSystemComponent::EnableEffect(const std::string& anEffectName)
{
	if (anEffectName == "") {
		for (unsigned int i = 0; i < myEffects.size(); ++i)
		{
			myEffects[i]->Enable();
		}
		return;
	}

	for (unsigned int i = 0; i < myEffects.size(); ++i)
	{
		if (myEffects[i]->myName == anEffectName)
		{
			myEffects[i]->Enable();
			return;
		}
	}
}

void CVFXSystemComponent::DisableEffect(const std::string& anEffectName)
{
	if (anEffectName == "") {
		for (unsigned int i = 0; i < myEffects.size(); ++i)
		{
			myEffects[i]->Disable();
		}
		return;
	}

	for (unsigned int i = 0; i < myEffects.size(); ++i)
	{
		if (myEffects[i]->myName == anEffectName)
		{
			myEffects[i]->Disable();
			return;
		}
	}
}

void CVFXSystemComponent::EnableEffect(const unsigned int anIndex)
{
	myEffects[anIndex]->Enable();
}

void CVFXSystemComponent::DisableEffect(const unsigned int anIndex)
{
	myEffects[anIndex]->Disable();
}