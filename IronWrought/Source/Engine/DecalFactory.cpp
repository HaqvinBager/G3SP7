#include "stdafx.h"
#include "DecalFactory.h"
#include "Engine.h"
#include <d3d11.h>
#include "Decal.h"
#include "MaterialHandler.h"

CDecalFactory* CDecalFactory::ourInstance = nullptr;

CDecalFactory* CDecalFactory::GetInstance()
{
    return ourInstance;
}

bool CDecalFactory::Init(CDirectXFramework* /*aFramework*/)
{
    return true;
}

void CDecalFactory::ClearFactory()
{
    auto itDecal = myDecalMap.begin();
    while (itDecal != myDecalMap.end())
    {
        itDecal->second = nullptr;
        ++itDecal;
    }
    myDecalMap.clear();
}

CDecal* CDecalFactory::GetDecal(std::string aFilePath)
{
    if (myDecalMap.find(aFilePath) == myDecalMap.end())
    {
        myDecalMapReferences[aFilePath] = 1;
        return LoadDecal(aFilePath);
    }

    myDecalMapReferences[aFilePath] += 1;

    return myDecalMap.at(aFilePath);
}

void CDecalFactory::ClearDecal(std::string aFilePath)
{
    if (myDecalMapReferences.find(aFilePath) != myDecalMapReferences.end())
    {
        myDecalMapReferences[aFilePath] -= 1;
        if (myDecalMapReferences[aFilePath] <= 0)
        {
            myDecalMapReferences[aFilePath] = 0;
            myDecalMap[aFilePath]->~CDecal();
            myDecalMap.erase(aFilePath);
        }
    }
}

CDecal* CDecalFactory::LoadDecal(std::string aFilePath)
{
    std::array<ID3D11ShaderResourceView*, 3> decalMaterial = CMainSingleton::MaterialHandler().RequestDecal(aFilePath);
    
    CDecal::SDecalData decalData;
    decalData.myMaterial = decalMaterial;
    decalData.myMaterialName = aFilePath;

    if (!decalMaterial[1])
    {
        decalData.myShouldRenderMaterial = false;
    }

    if (!decalMaterial[2])
    {
        decalData.myShouldRenderNormals = false;
    }

    CDecal* decal = new CDecal();
    decal->Init(decalData);

    myDecalMap.emplace(aFilePath, decal);
    return decal;
}

CDecalFactory::CDecalFactory()
{
    ourInstance = this;
}

CDecalFactory::~CDecalFactory()
{
    ourInstance = nullptr;
}
