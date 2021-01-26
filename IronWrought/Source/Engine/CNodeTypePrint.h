#pragma once
#include "CNodeType.h"
#include "NodeTypes.h"
class CNodeTypePrint : public CNodeType
{
public:
	CNodeTypePrint();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance) override;
	virtual std::string GetNodeName() { return "Print"; }

};

