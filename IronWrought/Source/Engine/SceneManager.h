#pragma once
#include "Scene.h"

class CSceneManager
{
public:
	CSceneManager();
	~CSceneManager();
	static CScene* CreateEmpty();
	static CScene* CreateScene(const std::string& aSceneName);//Temp


	//static CScene* CreateScene(std::string ajsonFile);//Temp

private:
	static bool AddGameObjects(CScene& aScene, const std::string& aJsonFileName);
	static void SetTransforms(CScene& aScene, const std::string& aJsonFileName);
	static void AddModelComponents(CScene& aScene, const std::string& aJsonFileName);
	static void AddInstancedModelComponents(CScene& aScene, const std::string& aJsonFileName);


	//static const std::string& Find(const std::vector<std::string>& someStrings, const std::string& aContains);
	//static std::string npos;


	//static CScene* CreateScene(std::vector<std::string> ajsonFile);
};

