#pragma once
#include "Scene.h"

class CSceneManager
{
public:
	CSceneManager();
	~CSceneManager();
	static CScene* CreateEmpty();
	static CScene* CreateScene(std::string ajsonFile);//Temp
	static CScene* CreateScene(std::vector<std::string> ajsonFile);
};

