#pragma once
#include "Scene.h"

class CSceneManager
{
public:
	CSceneManager();
	~CSceneManager();

	static CScene* CreateScene(std::string aJsonFile);



};

