#include "NodeType.h"
#include "NodeTypes.h"

class CNodeTypeBool : public CNodeType
{
public:
	CNodeTypeBool();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance) override;
	virtual std::string GetNodeName() { return "Bool"; }
	std::string GetNodeTypeCategory() override { return "Variables"; }
};
