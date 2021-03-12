#pragma once
#include "NodeTypes.h"

//class CNodeData;

class CUID
{

public:
	CUID(bool aCreateNewUID = true)
	{
		if (!aCreateNewUID)
		{
			return;
		}
		myGlobalUID++;
		myID = myGlobalUID;
		while (std::find(myAllUIDs.begin(), myAllUIDs.end(), myID) != myAllUIDs.end())
		{
			// Print warning, ID already in use :(
			myGlobalUID++;
			myID = myGlobalUID;
		}
		myAllUIDs.push_back(myID);
	}

	const unsigned int AsInt() const { return myID; }

	CUID& operator=(const CUID& other)
	{
		myID = other.myID;
#ifdef _DEBUG
		if (std::find(myAllUIDs.begin(), myAllUIDs.end(), myID) != myAllUIDs.end())
		{
			assert(0);
		}
#endif
		return *this;
	}
	CUID& operator=(const int other)
	{
		myID = other;
#ifdef _DEBUG
		if (std::find(myAllUIDs.begin(), myAllUIDs.end(), myID) != myAllUIDs.end())
		{
			assert(0);
		}
#endif
		return *this;
	}

	void SetUID(unsigned int aID)
	{
		myID = aID;
	}

	static std::vector<unsigned int> myAllUIDs;
	static unsigned int myGlobalUID;
private:
	unsigned int myID = 0;

};

struct SPin
{

	enum class EPinType
	{
		EFlow,
		EBool,
		EInt,
		EFloat,
		EString,
		EVector3,
		EList,
		EUnknown
	};

	enum class EPinTypeInOut
	{
		EPinTypeInOut_IN,
		EPinTypeInOut_OUT
	};


	SPin(std::string aText, EPinTypeInOut aType = EPinTypeInOut::EPinTypeInOut_IN, EPinType aVarType = EPinType::EFlow)
		:myText(aText)
	{
		myVariableType = aVarType;
		myPinType = aType;
	}

	SPin(const SPin& p2)
	{
		myText = p2.myText;
		myVariableType = p2.myVariableType;
		myPinType = p2.myPinType;
		myData = p2.myData;
		//myUID.SetUID(p2.myUID.AsInt()); // Cant do this here, copy constructor should create new UID and it will if we dont set it here
	}

	SPin& operator=(const SPin& p2)
	{
		myText = p2.myText;
		myVariableType = p2.myVariableType;
		myPinType = p2.myPinType;
		myData = p2.myData;
		myUID.SetUID(p2.myUID.AsInt()); // = operator should use the UID the last pin had, no new UID here, only in copy constructor
	}

	std::string myText;
	CUID myUID;
	EPinType myVariableType = EPinType::EFlow;
	NodeDataPtr myData = nullptr;
	EPinTypeInOut myPinType;
};

class CNodeType
{
public:
	virtual void ClearNodeInstanceFromMap(class CNodeInstance* aTriggeringNodeInstance);
	int DoEnter(class CNodeInstance* aTriggeringNodeInstance);
	std::string NodeName() { return myNodeName; }
	std::string NodeDataKey() { return myNodeDataKey; }
	void NodeName(std::string aNodeName) { myNodeName = aNodeName; }
	void NodeDataKey(std::string aNodeDataKey) { myNodeDataKey = aNodeDataKey; }

	//virtual void NodeData(CNodeData& aNodeData) { aNodeData; }

	std::vector<SPin> GetPins();
	virtual bool IsStartNode() { return false; }
	virtual std::string GetNodeTypeCategory() { return ""; }
	bool IsFlowNode()
	{
		for (auto& pin : myPins)
		{
			if (pin.myVariableType == SPin::EPinType::EFlow)
			{
				return true;
			}
		}
		return false;
	}
	// Draw debug? Draw text?
	virtual void DebugUpdate(class CNodeInstance*) {}

	int myID = -1;

protected:
	template <class T>
	void DeclareDataOnPinIfNecessary(SPin& aPin)
	{
		if (!aPin.myData)
		{
			aPin.myData = new T;
		}
	}
	virtual int OnEnter(class CNodeInstance* aTriggeringNodeInstance) = 0;
	void GetDataOnPin(CNodeInstance* aTriggeringNodeInstance, unsigned int aPinIndex, SPin::EPinType& outType, void*& someData, size_t& outSize);
	std::vector<SPin> myPins;
	std::string myNodeName = "N/A";
	std::string myNodeDataKey = "";
};

class CNodeTypeCollector
{
public:
	static void PopulateTypes();
	static CNodeType* GetNodeTypeFromID(unsigned int aClassID)
	{
		return myTypes[aClassID]; // 1:1 to nodetype enum
	}
	static CNodeType** GetAllNodeTypes()
	{
		return myTypes; // 1:1 to nodetype enum
	}
	static unsigned short GetNodeTypeCount()
	{
		return 	myTypeCounter; // 1:1 to nodetype enum
	}
	template <class T>
	static void RegisterType(std::string aNodeName)
	{
		myTypes[myTypeCounter] = new T;
		myTypes[myTypeCounter]->myID = myTypeCounter;
		myTypes[myTypeCounter]->NodeName(aNodeName);
		myTypeCounter++;
	}
	template <class T>
	static void RegisterDataType(std::string aNodeName, std::string aNodeDataKey)
	{
		myTypes[myTypeCounter] = new T;
		myTypes[myTypeCounter]->myID = myTypeCounter;
		myTypes[myTypeCounter]->NodeName(aNodeName);
		myTypes[myTypeCounter]->NodeDataKey(aNodeDataKey);
		myTypeCounter++;
	}

	static void RegisterNewDataType(std::string aNodeName, unsigned int aType);
	static CNodeType* myTypes[128];
	static unsigned short myTypeCounter;
	static unsigned short myTypeCount;
};