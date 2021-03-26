#pragma once
#include <map>
#include "VFXBase.h"

class CEngine;
class CDirectXFramework;

class CVFXMeshFactory {
	friend class CEngine;
public:
	bool Init(CDirectXFramework* aFramework);

	CVFXBase* LoadVFXBase(std::string aFilePath);
	CVFXBase* GetVFXBase(std::string aFilePath);

	static CVFXMeshFactory* GetInstance();

	std::vector<CVFXBase*> GetVFXBaseSet(std::vector<std::string> someFilePaths);
	std::vector<CVFXBase*> ReloadVFXBaseSet(std::vector<std::string> someFilePaths);

private:
	CVFXMeshFactory();
	~CVFXMeshFactory();

	void ReadJsonValues(std::string aFilePath, CVFXBase::SVFXBaseData& someVFXBaseData);

private:
	static CVFXMeshFactory* ourInstance;
	std::map<std::string, CVFXBase*> myVFXBaseMap;
	CDirectXFramework* myFramework;
};

