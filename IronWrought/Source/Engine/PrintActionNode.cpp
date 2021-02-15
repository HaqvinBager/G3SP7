#include <string>
#include <iostream>
#include "PrintActionNode.h"

CPrintActionNode::CPrintActionNode() : 
	CActionNode(),
	myTextToPrint("")
{
}

void CPrintActionNode::TakeAction()
{
	std::cout << myTextToPrint << std::endl;
}

void CPrintActionNode::TextToPrint(std::string aTextToPrint)
{
	myTextToPrint = aTextToPrint;
}
