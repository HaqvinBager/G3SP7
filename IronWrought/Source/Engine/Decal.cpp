#include "stdafx.h"
#include "Decal.h"
#include "MaterialHandler.h"

CDecal::CDecal()
	: myDecalData()
{
}

CDecal::~CDecal()
{
	for (unsigned int i = 0; i < myDecalData.myMaterialNames.size(); ++i)
	{
		CMainSingleton::MaterialHandler().ReleaseMaterial(myDecalData.myMaterialNames[i]);
		myDecalData.myMaterials[i][0] = nullptr;
		myDecalData.myMaterials[i][1] = nullptr;
		myDecalData.myMaterials[i][2] = nullptr;
	}
}

void CDecal::Init(SDecalData data)
{
	myDecalData = std::move(data);
}

CDecal::SDecalData& CDecal::GetDecalData()
{
	return myDecalData;
}
