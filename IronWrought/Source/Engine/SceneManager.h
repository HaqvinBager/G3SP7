#pragma once
#include "Scene.h"
#include "JsonReader.h"

struct SVertexPaintCollection;
class CSceneManager
{
public:
	CSceneManager();
	~CSceneManager();
	static CScene* CreateEmpty();
	static CScene* CreateScene(const std::string& aSceneName);
	static CScene* CreateMenuScene(const std::string& aSceneName, const std::string& aCanvasPath);

private:
	static bool AddGameObjects(CScene& aScene, RapidArray someData);
	static void SetTransforms(CScene& aScene, RapidArray someData);
	static void AddModelComponents(CScene& aScene, RapidArray someData);
	static void SetVertexPaintedColors(CScene& aScene, RapidArray someData, const SVertexPaintCollection& vertexColorData);

	static void AddInstancedModelComponents(CScene& aScene, RapidArray someData);
	static void AddPointLights(CScene& aScene, RapidArray someData);
	static void AddDecalComponents(CScene& aScene, RapidArray someData);
	static void AddPlayer(CScene& aScene);
};


#include <future>
class CSceneFactory {
	friend class CEngine;
public:
	static CSceneFactory* Get();
	void LoadSceneAsync(const std::string& aSceneName, std::function<void(std::string)> onComplete);
	void Update();

private:
	CSceneFactory();
	~CSceneFactory();
	static CSceneFactory* ourInstance;

private:
	std::future<CScene*> myFuture;
	std::function<void(std::string)> myOnComplete;
	std::string myLastSceneName;
};


