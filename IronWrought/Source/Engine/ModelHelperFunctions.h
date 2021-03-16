#pragma once

class CModelComponent;
class CGameObject;
class CModelHelperFunctions// Could just be a namespace
{
public:
	static bool LoadTintsToModelComponent(CModelComponent* aModelComponent, const std::string& aTintDataPath);
	static bool SaveTintsFromModelComponent(CModelComponent* aModelComponent, const std::string& aModelPath, const std::string& aTintDataPath = "");
	static CGameObject* CreateGameObjectWithTintedModel(const int anInstanceId, const std::string& aModelPath, const std::string& aTintDataPath);
};

