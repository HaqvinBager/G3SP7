#pragma once
#include "Behaviour.h"

#include "ParticleEmitter.h"
#include <queue>

class CGameObject;
class CVFXBase;
class CParticleEmitter;
class CVFXSystemComponent : public CBehaviour
{
public:
	CVFXSystemComponent(CGameObject& aParent, const std::string& aVFXDataPath);
	~CVFXSystemComponent() override;

	void Awake() override;
	void Start() override;
	void Update() override;
	void LateUpdate() override;

	void OnEnable() override;
	void OnDisable() override;

	void ResetParticles();

public:
	std::vector<CVFXBase*>& GetVFXBases() { return myVFXBases; }
	std::vector<Matrix>& GetVFXTransforms() { return myVFXTransforms; }

	std::vector<Matrix>& GetParticleTransforms() { return myEmitterTransforms; }
	std::vector<CParticleEmitter*>& GetParticleSet() { return myParticleEmitters; }
	std::vector<std::vector<CParticleEmitter::SParticleVertex>>& GetParticleVertices() { return myParticleVertices; }

private:
	void SpawnParticles(unsigned int anIndex, DirectX::SimpleMath::Vector3& aCameraPosition, CParticleEmitter::SParticleData& someParticleData, const Vector3& aTranslation, const float aScale);
	void UpdateParticles(unsigned int anIndex, DirectX::SimpleMath::Vector3& aCameraPosition, CParticleEmitter::SParticleData& particleData, const float aScale);

private:
	std::vector<CVFXBase*>	myVFXBases;
	std::vector<Matrix>		myVFXTransforms;
	std::vector<Matrix>		myVFXTransformsOriginal;
	std::vector<Vector3>	myVFXAngularSpeeds;
	std::vector<float>		myVFXDelays;
	std::vector<float>		myVFXDurations;
	std::vector<bool>		myVFXShouldOrbit;

	std::vector<CParticleEmitter*> myParticleEmitters;
	std::vector<Matrix>	 myEmitterTransforms;
	std::vector<Matrix>	 myEmitterTransformsOriginal;
	std::vector<Vector3> myEmitterAngularSpeeds;
	std::vector<std::vector<CParticleEmitter::SParticleVertex>> myParticleVertices;
	std::vector<std::queue<CParticleEmitter::SParticleVertex>>	myParticlePools;
	std::vector<float> myEmitterDelays;
	std::vector<float> myEmitterDurations;
	std::vector<float> myEmitterTimers;
	std::vector<bool>  myEmitterShouldOrbit;
};

