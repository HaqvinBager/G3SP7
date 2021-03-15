#include "stdafx.h"
#include "NodeInstance.h"
#include "NodeType.h"
#include <assert.h>
#include "GraphManager.h"
#include "BaseDecisionNode.h"

CNodeInstance::CNodeInstance(CGraphManager* aGraphManager, std::string aGraphKey, bool aCreateNewUID)
	:myGraphManager(aGraphManager), myGraphKey(aGraphKey), myUID(aCreateNewUID), myNodeType(nullptr), myEditorPosition{0.0f,0.0f}
{/*
	myEditorPosition[0] = 0.0f;
	myEditorPosition[1] = 0.0f;	*/
}


bool IsOutput(std::vector<SPin>& somePins, unsigned int anID)
{
	for (auto& pin : somePins)
	{
		if (pin.myPinType == SPin::EPinTypeInOut::EPinTypeInOut_OUT && pin.myUID.AsInt() == anID)
		{
			return true;
		}
	}
	return false;
}

void CNodeInstance::Enter()
{
	int outputIndex = myNodeType->DoEnter(this);
	outputIndex = outputIndex > -1 ? myPins[outputIndex].myUID.AsInt() : -1;
	if (outputIndex > -1)
	{
		for (auto& link : myLinks)
		{
			if ((int)link.myFromPinID == outputIndex && IsOutput(myPins, link.myFromPinID))
			{
				link.myLink->Enter();
			}
			else if (!IsOutput(myPins, link.myFromPinID) )
			{
				myGraphManager->ShowFlow(link.myLinkID);
			}
		}
	}
}

void CNodeInstance::ExitVia(unsigned int aPinIndex)
{
	SPin& pin = myPins[aPinIndex];
	std::vector<SNodeInstanceLink*> links = GetLinkFromPin(pin.myUID.AsInt());
	for (auto link : links)
	{
		link->myLink->Enter();
	}
}

void CNodeInstance::ConstructUniquePins()
{
	if (myNodeType != NULL)
		myPins = myNodeType->GetPins();
}

bool CNodeInstance::CanAddLink(unsigned int aPinIDFromMe)
{
	SPin* pin = GetPinFromID(aPinIDFromMe);

	if (pin->myPinType == SPin::EPinTypeInOut::EPinTypeInOut_IN &&  pin->myVariableType != SPin::EPinType::EFlow)
	{
		if (GetLinkFromPin(aPinIDFromMe).size() != 0)
		{
			return false;
		}
	}

	return true;
}

bool CNodeInstance::HasLinkBetween(unsigned int aFirstPin, unsigned int aSecondPin)
{
	for (auto link : myLinks)
	{
		if (link.myFromPinID == aFirstPin && link.myToPinID == aSecondPin
			||
			link.myToPinID== aFirstPin && link.myFromPinID == aSecondPin)
		{
			return true;
		}
	}
	return false;
}

bool CNodeInstance::AddLinkVia(CNodeInstance* aLink, unsigned int aPinIDFromMe, unsigned int aPinIDToMe, unsigned int aLinkID)
{
	if (aLink == nullptr)
		return false;

	if (aPinIDFromMe == 0)
		return false;

	if (aPinIDToMe == 0)
		return false;

	if (aLinkID == 0)
		return false;

	SPin* pin = GetPinFromID(aPinIDFromMe);

	if (pin->myPinType == SPin::EPinTypeInOut::EPinTypeInOut_IN &&  pin->myVariableType != SPin::EPinType::EFlow)
	{
		if (GetLinkFromPin(aPinIDFromMe).size() != 0)
		{
			return false;
		}
	}
	

	myLinks.push_back(SNodeInstanceLink(aLink, aPinIDFromMe, aPinIDToMe, aLinkID));
	return true;
}

void CNodeInstance::RemoveLinkToVia(CNodeInstance* aLink, unsigned int aPinThatIOwn)
{
	for (int i=0; i< myLinks.size(); i++)
	{
		if (myLinks[i].myLink == aLink)
		{
			if (myLinks[i].myFromPinID == aPinThatIOwn)
			{
				myLinks.erase(myLinks.begin() + i);
				return;
			}
			else if (myLinks[i].myToPinID == aPinThatIOwn)
			{
				myLinks.erase(myLinks.begin() + i);
				return;
			}
		}
	}
	assert(0);
}

bool CNodeInstance::IsPinConnected(SPin& aPin)
{
	return GetLinkFromPin(aPin.myUID.AsInt()).size() > 0;
}

std::string CNodeInstance::GetNodeName()
{
	return myNodeType->NodeName();
}

void CNodeInstance::ChangePinTypes(SPin::EPinType aType)
{
	for (auto& pin : myPins)
	{
		pin.myVariableType = aType;
	}
}

std::vector<SNodeInstanceLink*> CNodeInstance::GetLinkFromPin(unsigned int aPinToFetchFrom)
{
	std::vector< SNodeInstanceLink*> links;
	for (int i = 0; i < myLinks.size(); i++)
	{
		if (myLinks[i].myFromPinID == aPinToFetchFrom)
		{
			links.push_back(&myLinks[i]);
		}
		else if (myLinks[i].myToPinID == aPinToFetchFrom)
		{
			links.push_back(&myLinks[i]);
		}
	}
	return links;

}

SPin* CNodeInstance::GetPinFromID(unsigned int aUID)
{
	for (auto& pin : myPins)
	{
		if (pin.myUID.AsInt() == aUID)
		{
			return &pin;
		}
	}
	return nullptr;
}

int CNodeInstance::GetPinIndexFromPinUID(unsigned int aPinUID)
{
	for (int i = 0; i < myPins.size(); i++)
	{
		if (myPins[i].myUID.AsInt() == aPinUID)
		{
			return i;
		}
	}
	return -1;
}


void CNodeInstance::DebugUpdate()
{
	if (myNodeType)
	{
		myNodeType->DebugUpdate(this);
	}
}

void CNodeInstance::VisualUpdate(float aDeltaTime)
{
	myEnteredTimer -= aDeltaTime;
	if (myEnteredTimer <= 0.0f)
	{
		myEnteredTimer = 0.0f;
	}
}

std::vector<CGameObject*> CNodeInstance::GetCurrentGameObject()
{
	return myGraphManager->GetCurrentGameObject();
}

void CNodeInstance::FetchData(SPin::EPinType& anOutType, NodeDataPtr& someData, size_t& anOutSize, unsigned int aPinToFetchFrom)
{
	// If we dont have any data, but or link might have it, the link pin might have data written to it as well, then return that
	if (!myNodeType->IsFlowNode())
	{
		if (myPins[aPinToFetchFrom].myPinType == SPin::EPinTypeInOut::EPinTypeInOut_IN)
		{
			std::vector< SNodeInstanceLink*> links = GetLinkFromPin(myPins[aPinToFetchFrom].myUID.AsInt());
			if (links.size() > 0)
			{
				// Get data from first link, wierd if we have more than two links to fetch data from
				int pinIndex = links[0]->myLink->GetPinIndexFromPinUID(links[0]->myToPinID);
				if (pinIndex == -1)
				{
					assert(0);
				}
				
				myGraphManager->ShowFlow(links[0]->myLinkID);
				links[0]->myLink->FetchData(anOutType, someData, anOutSize, pinIndex);
				//we have a link in a node that is supposed only to store data, apparently this is connected aswell
				return;
			}
		}
		else
		{
			Enter();
		}

		
	}
	else
	{
		if (myPins[aPinToFetchFrom].myPinType == SPin::EPinTypeInOut::EPinTypeInOut_IN)
		{
			std::vector< SNodeInstanceLink*> links = GetLinkFromPin(myPins[aPinToFetchFrom].myUID.AsInt());
			if (links.size() > 0)
			{
				int indexInVector = -1;
				for (int i = 0; i < links[0]->myLink->myPins.size(); i++)
				{
					indexInVector = i;
					if (links[0]->myLink->myPins[i].myUID.AsInt() == links[0]->myToPinID) // links[0] always get the data from the first connection, more connections are illigal
					{
						break;
					}
				}

				assert(indexInVector != -1);
				links[0]->myLink->FetchData(anOutType, someData, anOutSize, indexInVector);
				return;
			}
		}
	}

	SPin& dataPin = myPins[aPinToFetchFrom];
	// We have the data, set ut to the return values
	if (dataPin.myVariableType == SPin::EPinType::EFloat)
	{
		anOutSize = dataPin.myData != nullptr ? sizeof(float) : 0;
	}
	else if (dataPin.myVariableType == SPin::EPinType::EInt)
	{
		anOutSize = dataPin.myData != nullptr ? sizeof(int) : 0;
	}
	else if (dataPin.myVariableType == SPin::EPinType::EBool)
	{
		anOutSize = dataPin.myData != nullptr ? sizeof(bool) : 0;
	}
	else if (dataPin.myVariableType == SPin::EPinType::EString)
	{
		anOutSize = dataPin.myData != nullptr ? strlen(static_cast<char*>(dataPin.myData)) : 0;
	}
	else if (dataPin.myVariableType == SPin::EPinType::EVector3)
	{
		anOutSize = dataPin.myData != nullptr ? sizeof(DirectX::SimpleMath::Vector3) : 0;
	}
	else if (dataPin.myVariableType == SPin::EPinType::EStringList)
	{
		anOutSize = dataPin.myData != nullptr ? strlen(static_cast<char*>(dataPin.myData)) : 0;
	}
	
	someData = dataPin.myData;
	anOutType = dataPin.myVariableType;
}