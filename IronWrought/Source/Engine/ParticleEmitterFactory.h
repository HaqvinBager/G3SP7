#pragma once
#include "ParticleEmitter.h"
#include <map>

class CDirectXFramework;

class CParticleEmitterFactory
{
public:
	CParticleEmitterFactory();
	~CParticleEmitterFactory();

	bool Init(CDirectXFramework* aFramework);

	CParticleEmitter* LoadParticle(std::string aFilePath);
	CParticleEmitter* GetParticle(std::string aFilePath);

	std::vector<CParticleEmitter*> GetParticleSet(std::vector<std::string> someFilePaths);
	std::vector<CParticleEmitter*> ReloadParticleSet(std::vector<std::string> someFilePaths);

public:
	static CParticleEmitterFactory* GetInstance();
	ID3D11ShaderResourceView* GetShaderResourceView(ID3D11Device* aDevice, std::string aTexturePath);
	void ReadJsonValues(std::string aFilePath, CParticleEmitter::SParticleData& someParticleData);

private:
	static CParticleEmitterFactory* ourInstance;

private:
	ID3D11Device* myDevice;
	std::map<std::string, CParticleEmitter*> myParticleEmitters;
};

