#include "stdafx.h"
#include "LightFactory.h"
#include "Engine.h"
#include "EnvironmentLight.h"
#include "PointLight.h"
#include "SpotLight.h"

CLightFactory* CLightFactory::ourInstance = nullptr;
CLightFactory* CLightFactory::GetInstance()
{
    return ourInstance;
}
bool CLightFactory::Init(CEngine& anEngine)
{
    myEngine = &anEngine;
    return true;
}
CEnvironmentLight* CLightFactory::CreateEnvironmentLight(std::string aCubeMapPath)
{
	CEnvironmentLight* light = new CEnvironmentLight();
    ENGINE_ERROR_BOOL_MESSAGE(light->Init(myEngine->myFramework, aCubeMapPath), "Environment Light could not be initialized.");
    
    return light;
}

CPointLight* CLightFactory::CreatePointLight() {
    CPointLight* pointLight = new CPointLight();
    ENGINE_ERROR_BOOL_MESSAGE(pointLight->Init(), "Point Light could not be initialized.");

    return pointLight;
}

CSpotLight* CLightFactory::CreateSpotLight()
{
    CSpotLight* spotLight = new CSpotLight();
    ENGINE_ERROR_BOOL_MESSAGE(spotLight->Init(), "Spot Light could not be initialized.");
    
    return spotLight;
}

CLightFactory::CLightFactory()
{
    ourInstance = this;
    myEngine = nullptr;
}

CLightFactory::~CLightFactory()
{
    ourInstance = nullptr;
    myEngine = nullptr;
}
