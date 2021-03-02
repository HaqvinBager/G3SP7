#pragma once
#include <map>
#include "VFXBase.h"

class CEngine;
class CDirectXFramework;

class CVFXFactory {
	friend class CEngine;
public:
	bool Init(CDirectXFramework* aFramework);

	CVFXBase* LoadVFXBase(std::string aFilePath);
	CVFXBase* GetVFXBase(std::string aFilePath);

	static CVFXFactory* GetInstance();

	std::vector<CVFXBase*> GetVFXBaseSet(std::vector<std::string> someFilePaths);

private:
	CVFXFactory();
	~CVFXFactory();

	void ReadJsonValues(std::string aFilePath, CVFXBase::SVFXBaseData& someVFXBaseData);

private:
	static CVFXFactory* ourInstance;
	std::map<std::string, CVFXBase*> myVFXBaseMap;
	CDirectXFramework* myFramework;
};

