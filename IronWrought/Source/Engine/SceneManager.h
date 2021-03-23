#pragma once
#include "Scene.h"
#include "JsonReader.h"

class CSceneManager
{
public:
	CSceneManager();
	~CSceneManager();
	static CScene* CreateEmpty();
	static CScene* CreateScene(const std::string& aSceneName);

private:
	
	//static bool AddGameObjects(CScene& aScene, rapidjson::GenericObject<false, rapidjson::Value>& aSceneObjectaJsonFileName);

	static bool AddGameObjects(CScene& aScene, RapidArray someData);
	static void SetTransforms(CScene& aScene, RapidArray someData);
	static void AddModelComponents(CScene& aScene, RapidArray someData);

	//static bool AddGameObjects(CScene& aScene, const std::string& aJsonFileName);
	static void AddInstancedModelComponents(CScene& aScene, RapidArray someData);
	static void AddPointLights(CScene& aScene, RapidArray someData);
	static void AddDecalComponents(CScene& aScene, RapidArray someData);
	static void AddPlayer(CScene& aScene);
};

