#pragma once
#include "ActionNode.h"

class CPrintActionNode : public CActionNode
{
public:
	CPrintActionNode();
	void TakeAction() override;
	void TextToPrint(std::string aTextToPrint);
private:
	std::string myTextToPrint;
};