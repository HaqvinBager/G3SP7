#pragma once
#include "NodeTypes.h"
#include "hasher.h"
#include "NodeDataManager.h"

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
		myAllUIDs.push_back(myID);
		myGlobalUID = myID;
	}

	static void ClearUIDS()
	{
		myGlobalUID = 0;
		myAllUIDs.clear();
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
		EStringList,
		EStringListIndexed,
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

	SPin(const SPin& aPinToCopy)
	{
		myText = aPinToCopy.myText;
		myVariableType = aPinToCopy.myVariableType;
		myPinType = aPinToCopy.myPinType;
		myData = aPinToCopy.myData;
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
	enum class ENodeType
	{
		EDefault,
		ECustom,
		EChild
	};

	virtual void ClearNodeInstanceFromMap(class CNodeInstance* aTriggeringNodeInstance);
	int DoEnter(class CNodeInstance* aTriggeringNodeInstance);
	std::string NodeName() { return myNodeName; }
	std::string NodeDataKey() { return myNodeDataKey; }
	void NodeName(std::string aNodeName) { myNodeName = aNodeName; }
	void NodeDataKey(std::string aNodeDataKey) { myNodeDataKey = aNodeDataKey; }

	//virtual void NodeData(CNodeData& aNodeData) { aNodeData; }

	std::vector<SPin> GetPins();
	virtual bool IsStartNode() { return false; }
	virtual bool IsInputNode() { return false; }
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
	ENodeType myNodeType = ENodeType::EDefault;

protected:
	template <class T>
	void DeclareDataOnPinIfNecessary(SPin& aPin)
	{
		if (!aPin.myData)
		{
			aPin.myData = new T;
		}
	}
	template <class T>
	void DeclareDataOnPinIfNecessary(SPin& aPin, const T aValue)
	{
		if (!aPin.myData)
		{
			aPin.myData = new T(aValue);
		}
	}
	virtual int OnEnter(class CNodeInstance* aTriggeringNodeInstance) = 0;
	void GetDataOnPin(CNodeInstance* aTriggeringNodeInstance, unsigned int aPinIndex, SPin::EPinType& anOutType, void*& someData, size_t& anOutSize);
	std::vector<SPin> myPins;
	std::string myNodeName = "N/A";
	std::string myNodeDataKey = "";
};

struct SNodeTypeData
{
	CNodeType* myTypes[128];
	unsigned short myTypeCounter;
};

class CNodeTypeCollector
{
public:

	static void PopulateTypes();

	static CNodeType* GetNodeTypeFromID(unsigned int aClassID, CNodeType::ENodeType aNodeType = CNodeType::ENodeType::EDefault)
	{
		switch (aNodeType)
		{
		case CNodeType::ENodeType::EDefault:
			return myDefaultTypes[aClassID];
		case CNodeType::ENodeType::ECustom:
			return myCustomTypes[aClassID];
		case CNodeType::ENodeType::EChild:
			return myChildTypes[aClassID];
		}

		return nullptr;
	}

	static CNodeType** GetAllNodeTypes(CNodeType::ENodeType aNodeType = CNodeType::ENodeType::EDefault)
	{
		switch (aNodeType)
		{
		case CNodeType::ENodeType::EDefault:
			return myDefaultTypes;
		case CNodeType::ENodeType::ECustom:
			return myCustomTypes;
		case CNodeType::ENodeType::EChild:
			return myChildTypes;
		}

		return nullptr;
	}

	static unsigned short GetNodeTypeCount(CNodeType::ENodeType aNodeType = CNodeType::ENodeType::EDefault)
	{
		switch (aNodeType)
		{
		case CNodeType::ENodeType::EDefault:
			return myDefaultTypeCounter;
		case CNodeType::ENodeType::ECustom:
			return myCustomTypeCounter;
		case CNodeType::ENodeType::EChild:
			return myChildTypeCounter;
		}
		 // 1:1 to nodetype enum
		return NULL;
	}

	template <class T>
	static void RegisterType(const std::string& aNodeName, CNodeType::ENodeType aNodeType = CNodeType::ENodeType::EDefault)
	{
		if (aNodeType == CNodeType::ENodeType::EDefault)
		{
			myDefaultTypes[myDefaultTypeCounter] = new T;
			myDefaultTypes[myDefaultTypeCounter]->myID = myDefaultTypeCounter;
			myDefaultTypes[myDefaultTypeCounter]->NodeName(aNodeName);
			myDefaultTypes[myDefaultTypeCounter]->myNodeType = aNodeType;
			myDefaultTypeCounter++;
		}
		else if (aNodeType == CNodeType::ENodeType::EChild)
		{
			myChildTypes[myChildTypeCounter] = new T;
			myChildTypes[myChildTypeCounter]->myID = myChildTypeCounter;
			myChildTypes[myChildTypeCounter]->NodeName(aNodeName);
			myChildTypes[myChildTypeCounter]->myNodeType = aNodeType;
			myChildTypeCounter++;
		}
	}

	template <class T>
	static void RegisterDataType(const std::string& aNodeName, const std::string& aNodeDataKey)
	{
		myCustomTypes[myCustomTypeCounter] = new T;
		myCustomTypes[myCustomTypeCounter]->myID = myCustomTypeCounter;
		myCustomTypes[myCustomTypeCounter]->NodeName(aNodeName);
		myCustomTypes[myCustomTypeCounter]->NodeDataKey(aNodeDataKey);
		myCustomTypes[myCustomTypeCounter]->myNodeType = CNodeType::ENodeType::ECustom;
		myCustomTypeCounter++;
	}

	static void DeregisterDataType(const std::string& aNodeName, const std::string& aNodeDataKey)
	{
		int i;

		size_t hash = Hasher::GetHashValue(aNodeName);

		for (i = 0; i < myCustomTypeCounter; ++i)
		{
			if (Hasher::GetHashValue(myCustomTypes[i]->NodeName()) == hash)
			{
				break;
			}
		}

		if (i < myCustomTypeCounter)
		{
			for (int j = i; j < myCustomTypeCounter; ++j)
			{
				myCustomTypes[j] = myCustomTypes[j + 1];
				
				if (myCustomTypes[j] == NULL)
					break;
				
				myCustomTypes[j]->myID--;
			}
			--myCustomTypeCounter;
			CNodeDataManager::Get()->RemoveData(aNodeDataKey);
		}
	}

	static void RegisterNewDataType(const std::string& aNodeName, unsigned int aType);
	static void DegisterCustomDataType(const std::string& aNodeName);

	static CNodeType* myDefaultTypes[128];
	static CNodeType* myCustomTypes[128];
	static CNodeType* myChildTypes[128];
	static unsigned short myDefaultTypeCounter;
	static unsigned short myCustomTypeCounter;
	static unsigned short myChildTypeCounter;

public:
	static void RegisterChildNodeTypes(std::string aKey, const unsigned int anIndex, int aGOID);
};