#pragma once
#include "Behaviour.h"
#include "ParticleEmitter.h"
#include <SimpleMath.h>
#include <queue>

class CGameObject;

class CParticleEmitterComponent : public CBehaviour
{
public:
	CParticleEmitterComponent(CGameObject& aParent);
	~CParticleEmitterComponent() override;

	void Awake() override;
	void Start() override;
	void Update() override;

	void Init(std::vector<CParticleEmitter*> someParticles);
	void SetTransform(DirectX::SimpleMath::Vector3 aPosition, DirectX::SimpleMath::Vector3 aRotation);
	void SetRotation(DirectX::SimpleMath::Vector3 aRotation);
	void SetRotation(DirectX::SimpleMath::Quaternion aQuaternion);
	void SetPosition(DirectX::SimpleMath::Vector3 aPosition);
	void SetScale(float aScale);
	void Move(DirectX::SimpleMath::Vector3 aMovement);
	void Rotate(DirectX::SimpleMath::Vector3 aRotation);
	void Rotate(DirectX::SimpleMath::Quaternion aQuaternion);

	void Update(DirectX::SimpleMath::Vector3 aCameraPosition);

	void OnEnable() override;
	void OnDisable() override;
	void Reset();
public:
	std::vector<CParticleEmitter*>& GetParticleSet() { return myParticleEmitters; }
	std::vector<std::vector<CParticleEmitter::SParticleVertex>>& GetParticleVertices() { return myParticleVertices; }
	DirectX::SimpleMath::Matrix GetTransform() { return myTransform; }
	const std::vector<float> EmitterDurations() const { return myEmitterDurations; }
private:
	void SpawnParticles(unsigned int anIndex, DirectX::SimpleMath::Vector3& aCameraPosition, CParticleEmitter::SParticleData& particleData);
	void UpdateParticles(unsigned int anIndex, DirectX::SimpleMath::Vector3& aCameraPosition, CParticleEmitter::SParticleData& particleData);

private:
	DirectX::SimpleMath::Matrix myTransform;
	std::vector<CParticleEmitter*> myParticleEmitters;
	std::vector<std::vector<CParticleEmitter::SParticleVertex>> myParticleVertices;
	std::vector<std::queue<CParticleEmitter::SParticleVertex>> myParticlePools;
	std::vector<float> myEmitterTimers;
	std::vector<float> myEmitterDelays;
	std::vector<float> myEmitterDurations;
	
	float myScale = 1.0f;// Obsolete
};

