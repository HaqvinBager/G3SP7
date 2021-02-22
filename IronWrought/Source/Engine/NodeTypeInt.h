#include "NodeType.h"
#include "NodeTypes.h"

class CNodeTypeInt : public CNodeType
{
public:
	CNodeTypeInt();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance) override;
	virtual std::string GetNodeName() { return "Int"; }
	std::string GetNodeTypeCategory() override { return "Variables"; }
};
