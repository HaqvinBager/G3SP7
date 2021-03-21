#include "stdafx.h"
#include "BlendTree.h"
#include "AnimationController.h"

CBlendTree::CBlendTree(const std::string& aName, CAnimationController& aController) 
	: myName(aName)
	, myController(aController)
	, myBlendValue(0.0f)
{
}

CBlendTree::~CBlendTree()
{
}

void CBlendTree::Update(const float /*aTime*/)
{
	for (int i = 0; i < myMotions.size(); ++i)
	{
		int index = myController.AnimationIndex(myMotions[i]);
		float tps = myController.AnimationTPS(index);
		float duration = myController.AnimationDurationInTicks(index);

		index;
		tps;
		duration;
	}
}

//const std::string& CBlendTree::GetMotion(const int anIndex, const float aTime) const
//{
//
//
//
//	// TODO: insert return statement here
//}
