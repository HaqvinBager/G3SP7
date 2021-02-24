#include "stdafx.h"
#include "Decal.h"
#include "MaterialHandler.h"

CDecal::CDecal()
	: myDecalData()
{
}

CDecal::~CDecal()
{
	CMainSingleton::MaterialHandler().ReleaseMaterial(myDecalData.myMaterialName);
	myDecalData.myMaterial[0] = nullptr;
	myDecalData.myMaterial[1] = nullptr;
	myDecalData.myMaterial[2] = nullptr;
}

void CDecal::Init(SDecalData data)
{
	myDecalData = std::move(data);
}

CDecal::SDecalData& CDecal::GetDecalData()
{
	return myDecalData;
}
