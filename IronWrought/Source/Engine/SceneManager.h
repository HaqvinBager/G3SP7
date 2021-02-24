#pragma once
#include "Scene.h"
#include "JsonReader.h"

class CSceneManager
{
public:
	CSceneManager();
	~CSceneManager();
	static CScene* CreateEmpty();
	static CScene* CreateScene(std::string ajsonFile);//Temp
	static CScene* CreateScene(const std::vector<std::string>& someJsonFiles);//Temp

private:
	static bool IsValid(const rapidjson::Document& aDoc);


	static bool AddGameObjects(CScene& aScene, const std::vector<std::string>& someJsonFiles);
	static void SetTransforms(CScene& aScene, const std::vector<std::string>& someJsonFiles);
	static void AddModelComponents(CScene& aScene, const std::vector<std::string>& someJsonFiles);

	//static CScene* CreateScene(std::vector<std::string> ajsonFile);
};

