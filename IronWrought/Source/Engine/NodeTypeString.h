#include "NodeType.h"
#include "NodeTypes.h"

class CNodeTypeString : public CNodeType
{
public:
	CNodeTypeString();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance) override;
	virtual std::string GetNodeName() { return "Storage"; }
	std::string GetNodeTypeCategory() override { return "Math"; }
};
