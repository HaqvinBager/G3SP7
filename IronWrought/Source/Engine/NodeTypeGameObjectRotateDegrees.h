#pragma once
#include "NodeType.h"
#include "NodeTypes.h"


class CNodeTypeGameObjectRotateDegrees : public CNodeType
{
private:
	struct SPinData
	{
		Vector3 myInput;
		bool myIsFinished;
	};
public:
	CNodeTypeGameObjectRotateDegrees();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance) override;
	std::string GetNodeTypeCategory() override { return "Game Object"; }

private:
	std::map<int, Quaternion> myRotations;
	std::map<int, bool> myIsFinished;
	Vector3 myInput;
};

