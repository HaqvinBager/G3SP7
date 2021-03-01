#pragma once
#include "ParticleEmitter.h"
#include <map>

class CDirectXFramework;

class CParticleFactory
{
public:
	CParticleFactory();
	~CParticleFactory();

	bool Init(CDirectXFramework* aFramework);

	CParticleEmitter* LoadParticle(std::string aFilePath);
	CParticleEmitter* GetParticle(std::string aFilePath);

	std::vector<CParticleEmitter*> GetParticleSet(std::vector<std::string> someFilePaths);

public:
	static CParticleFactory* GetInstance();
	ID3D11ShaderResourceView* GetShaderResourceView(ID3D11Device* aDevice, std::string aTexturePath);
	void ReadJsonValues(std::string aFilePath, CParticleEmitter::SParticleData& someParticleData);

private:
	static CParticleFactory* ourInstance;

private:
	ID3D11Device* myDevice;
	std::map<std::string, CParticleEmitter*> myParticleEmitters;
};

