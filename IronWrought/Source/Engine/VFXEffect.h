#pragma once
#include <queue>

#include "ParticleEmitter.h"

class CVFXBase;

struct SVFXEffect {
	std::string myName = "";

	std::vector<CVFXBase*>	myVFXBases;
	std::vector<Matrix>		myVFXTransforms;
	std::vector<Matrix>		myVFXTransformsOriginal;
	std::vector<Vector3>	myVFXAngularSpeeds;
	std::vector<float>		myVFXDelays;
	std::vector<float>		myVFXDurations;
	std::vector<float>		myVFXBaseDelays;
	std::vector<float>		myVFXBaseDurations;
	std::vector<bool>		myVFXShouldOrbit;
	std::vector<bool>		myVFXIsActive;

	std::vector<CParticleEmitter*> myParticleEmitters;
	std::vector<Matrix>	 myEmitterTransforms;
	std::vector<Matrix>	 myEmitterTransformsOriginal;
	std::vector<Vector3> myEmitterAngularSpeeds;
	std::vector<std::vector<CParticleEmitter::SParticleVertex>> myParticleVertices;
	std::vector<std::queue<CParticleEmitter::SParticleVertex>>	myParticlePools;
	std::vector<float> myEmitterDelays;
	std::vector<float> myEmitterDurations;
	std::vector<float> myEmitterBaseDelays;
	std::vector<float> myEmitterBaseDurations;
	std::vector<float> myEmitterTimers;
	std::vector<bool>  myEmitterShouldOrbit;
	
	

	bool myIsEnabled;

	std::vector<CVFXBase*>& GetVFXBases() { return myVFXBases; }
	std::vector<Matrix>& GetVFXTransforms() { return myVFXTransforms; }
	std::vector<bool>& GetVFXIsActive() { return myVFXIsActive; }

	std::vector<Matrix>& GetParticleTransforms() { return myEmitterTransforms; }
	std::vector<CParticleEmitter*>& GetParticleSet() { return myParticleEmitters; }
	std::vector<std::vector<CParticleEmitter::SParticleVertex>>& GetParticleVertices() { return myParticleVertices; }

	void SpawnParticles(unsigned int anIndex, DirectX::SimpleMath::Vector3& aCameraPosition, CParticleEmitter::SParticleData& someParticleData, const Vector3& aTranslation, const float aScale);
	void UpdateParticles(unsigned int anIndex, DirectX::SimpleMath::Vector3& aCameraPosition, CParticleEmitter::SParticleData& particleData, const float aScale);
	
	float Lerp(float a, float b, float t) const
	{
		return a + t * (b - a);
	}
	
	float InverseLerp(float a, float b, float t) const
	{
		return (t - a) / (b - a);
	}

	float Remap(const float iMin, const float iMax, const float oMin, const float oMax, const float v) const
	{
		float t = InverseLerp(iMin, iMax, v);
		return Lerp(oMin, oMax, t);
	}
	
	const float CalculateInterpolator(const std::vector<Vector2>& somePoints, const float t) const;

	void ResetParticles();

	void Enable();
	void Disable();
};

