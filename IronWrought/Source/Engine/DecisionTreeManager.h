#pragma once

class IBaseDecisionNode;

class CDecisionTreeManager
{
public:
	~CDecisionTreeManager();
	static CDecisionTreeManager* Get();
	void RunTree(IBaseDecisionNode* aRootNode);
	int RequestID();
	void AddNode(IBaseDecisionNode* aNodeToAdd);
	IBaseDecisionNode* GetNodeFromID(int aNodeID);
	void RemoveNodeWithID(int aNodeID);
private:
	CDecisionTreeManager();
	std::unordered_map<int, IBaseDecisionNode*> myTreeMap;
	int myIDCounter;

	static CDecisionTreeManager* ourInstance;
};

