#include "stdafx.h"
#include "ParticleEmitter.h"

CParticleEmitter::CParticleEmitter() : myParticleData()
{
}

CParticleEmitter::~CParticleEmitter()
{
}

void CParticleEmitter::Init(CParticleEmitter::SParticleData someParticleData)
{
	myParticleData = someParticleData;
}
