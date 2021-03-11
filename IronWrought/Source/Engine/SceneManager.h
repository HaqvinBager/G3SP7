#pragma once
#include "Scene.h"

class CSceneManager
{
public:
	CSceneManager();
	~CSceneManager();
	static CScene* CreateEmpty();
	static CScene* CreateScene(const std::string& aSceneName);

private:
	static bool AddGameObjects(CScene& aScene, const std::string& aJsonFileName);
	static void SetTransforms(CScene& aScene, const std::string& aJsonFileName);
	static void AddModelComponents(CScene& aScene, const std::string& aJsonFileName);
	static void AddInstancedModelComponents(CScene& aScene, const std::string& aJsonFileName);
	static void AddPointLights(CScene& aScene, const std::string& aJsonFileName);
	static void AddDecalComponents(CScene& aScene, const std::string& aJsonFileName);
	static void AddPlayer(CScene& aScene, const std::string& aJsonFileName);
};

