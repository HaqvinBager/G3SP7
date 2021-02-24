#include "NodeType.h"
#include "NodeTypes.h"

class CNodeTypeFloat : public CNodeType
{
public:
	CNodeTypeFloat();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance) override;
	virtual std::string GetNodeName() { return "Float"; }
	std::string GetNodeTypeCategory() override { return "Variables"; }
};
