#include "NodeType.h"
#include "NodeTypes.h"

class CNodeTypeVariableGetFloat : public CNodeType
{
public:
	CNodeTypeVariableGetFloat();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance) override;
	std::string GetNodeTypeCategory() override { return "Get Data"; }
};
