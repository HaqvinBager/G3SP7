#pragma once
#include "NodeType.h"
#include "NodeTypes.h"

/*
Hello and welcome to the most importand nodetype in our life, the start node
To start the execution of the whole nodetree we need a starting point, this node is a starting point
We find this and call enter on it, the rest will be handled by he connections to this node and it will be self sustaining, much like Skynet
*/
class CNodeTypeStart : public CNodeType
{
public:
	CNodeTypeStart();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance) override;
	bool IsStartNode() override { return true; }
	virtual std::string GetNodeName() { return "Start"; }
	
	
};

