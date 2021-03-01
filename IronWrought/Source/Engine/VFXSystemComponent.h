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
	CVFXSystemComponent(CGameObject& aParent, const std::vector<std::string>& someVFXPaths, const std::vector<Matrix>& someTransforms, const std::vector<CParticleEmitter*>& someParticles);
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

	std::vector<Matrix>& GetParticleTransforms() { return myParticleTransforms; }
	std::vector<CParticleEmitter*>& GetParticleSet() { return myParticleEmitters; }
	std::vector<std::vector<CParticleEmitter::SParticleVertex>>& GetParticleVertices() { return myParticleVertices; }

private:
	std::vector<float>		myVFXDelays;
	std::vector<float>		myVFXDurations;
	std::vector<CVFXBase*>	myVFXBases;
	std::vector<Matrix>		myVFXTransforms;
	std::vector<Matrix>		myVFXTransformsOriginal;

	void SpawnParticles(unsigned int anIndex, DirectX::SimpleMath::Vector3& aCameraPosition, CParticleEmitter::SParticleData& someParticleData);
	void UpdateParticles(unsigned int anIndex, DirectX::SimpleMath::Vector3& aCameraPosition, CParticleEmitter::SParticleData& particleData);
	std::vector<CParticleEmitter*> myParticleEmitters;
	std::vector<Matrix>	myParticleTransforms;
	std::vector<Matrix>	myParticleTransformsOriginal;
	std::vector<std::vector<CParticleEmitter::SParticleVertex>> myParticleVertices;
	std::vector<std::queue<CParticleEmitter::SParticleVertex>>	myParticlePools;
	std::vector<float> myEmitterTimers;
	std::vector<float> myEmitterDelays;
	std::vector<float> myEmitterDurations;
};

