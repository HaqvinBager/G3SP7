#pragma once

class IBaseDecisionNode
{
public:
	//Recursively walk through the tree
	virtual IBaseDecisionNode* MakeDecision() = 0;
	virtual void InvalidateTree() = 0;

	inline int ID() { return myID; }
	inline int IsTrueNodeToID() { return myIsTrueNodeToID; }
	inline int IsFalseNodeToID() { return myIsFalseNodeToID; }
	inline bool TreeIsComplete() { return myTreeIsComplete; }
	virtual IBaseDecisionNode* Parent() { return myParent; }

	inline void ID(int anID) { myID = anID; }
	inline void IsTrueNodeToID(int anIsTrueNodeToID) { myIsTrueNodeToID = anIsTrueNodeToID; }
	inline void IsFalseNodeToID(int anIsFalseNodeToID) { myIsFalseNodeToID = anIsFalseNodeToID; }
	inline void TreeIsComplete(bool aTreeIsComplete) { myTreeIsComplete = aTreeIsComplete; }
	inline void Parent(IBaseDecisionNode* aParent) { myParent = aParent; }

private:
	IBaseDecisionNode* myParent = nullptr;
	int myID = 0;
	int myIsTrueNodeToID = 0;
	int myIsFalseNodeToID = 0;
	//bool myIsRoot = true;
	bool myTreeIsComplete = false;
};