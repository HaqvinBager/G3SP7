#pragma once

#include "..\Includes\rapidjson\document.h"
class CModelComponent;
class CGameObject;
class CModelHelperFunctions// Could just be a namespace
{
public:
	// Load using CModelComponent
	static bool LoadTintsToModelComponent(CModelComponent* aModelComponent, const std::string& aTintDataPath);
	static bool LoadTintsToModelComponent(CModelComponent* aModelComponent, const std::string& aTintDataPath, std::string& anOutModelPath);
	static bool LoadTintsToModelComponent(CModelComponent* aModelComponent, rapidjson::Document& aDocument);
	static bool LoadTintsToModelComponent(CModelComponent* aModelComponent, rapidjson::Document& aDocument, std::string& anOutModelPath);
	// Load using CGameObject
	static bool LoadTintsToModelComponent(CGameObject* aGameObject, const std::string& aTintDataPath);
	static bool LoadTintsToModelComponent(CGameObject* aGameObject, const std::string& aTintDataPath, std::string& anOutModelPath);

	static bool SaveTintsFromModelComponent(CModelComponent* aModelComponent, const std::string& aModelPath, const std::string& aTintDataPath = "");
	static bool SaveTintsFromModelComponent(CGameObject* aGameObject, const std::string& aModelPath, const std::string& aTintDataPath = "");
	
	static CGameObject* CreateGameObjectWithTintedModel(const int anInstanceId, const std::string& aModelPath, const std::string& aTintDataPath);

	static bool AddModelComponentWithTintsFromData(CGameObject* aGameObject, const std::string& aTintDataPath);
	static bool AddModelComponentWithTintsFromData(CGameObject* aGameObject, const std::string& aTintDataPath, std::string& anOutModelPath);
private:
	//Functions for copypasted code :)
};

