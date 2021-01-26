#include "stdafx.h"
#include "CNodeInstance.h"
#include "CNodeType.h"
#include <assert.h>
#include "CGraphManager.h"


CNodeInstance::CNodeInstance(bool aCreateNewUID)
	:myUID(aCreateNewUID)
{

	
}

bool IsOutput(std::vector<SPin>& pins, unsigned int aID)
{
	for (auto& pin : pins)
	{
		if (pin.myPinType == SPin::PinTypeInOut::PinTypeInOut_OUT && pin.myUID.AsInt() == aID)
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
				CGraphManager::ShowFlow(link.myLinkID);
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
	myPins = myNodeType->GetPins();

}

bool CNodeInstance::CanAddLink(unsigned int aPinIdFromMe)
{
	SPin* pin = GetPinFromID(aPinIdFromMe);

	if (pin->myPinType == SPin::PinTypeInOut::PinTypeInOut_IN &&  pin->myVariableType != SPin::PinType::Flow)
	{
		if (GetLinkFromPin(aPinIdFromMe).size() != 0)
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
			link.myToPinID== aFirstPin && link.myFromPinID == aSecondPin
			)
		{
			return true;
		}
	}
	return false;
}

bool CNodeInstance::AddLinkToVia(CNodeInstance* aLink, unsigned int aPinIdFromMe, unsigned int aPinIdToMe, unsigned int aLinkID)
{
	SPin* pin = GetPinFromID(aPinIdFromMe);

	if (pin->myPinType == SPin::PinTypeInOut::PinTypeInOut_IN &&  pin->myVariableType != SPin::PinType::Flow)
	{
		if (GetLinkFromPin(aPinIdFromMe).size() != 0)
		{
			return false;
		}
	}
	

	myLinks.push_back(SNodeInstanceLink(aLink, aPinIdFromMe, aPinIdToMe, aLinkID));
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
	return myNodeType->GetNodeName();
}

void CNodeInstance::ChangePinTypes(SPin::PinType aType)
{
	for (auto& pin : myPins)
	{
		pin.myVariableType = aType;
	}
}

std::vector< SNodeInstanceLink*> CNodeInstance::GetLinkFromPin(unsigned int aPinToFetchFrom)
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

void CNodeInstance::VisualUpdate(float aTimeDelta)
{
	myEnteredTimer -= aTimeDelta;
	if (myEnteredTimer <= 0.0f)
	{
		myEnteredTimer = 0.0f;
	}
}

void CNodeInstance::FetchData(SPin::PinType& outType, NodeDataPtr& someData, size_t& outSize, unsigned int aPinToFetchFrom)
{
	// If we dont have any data, but or link might have it, the link pin might have data written to it as well, then return that
	if (!myNodeType->IsFlowNode())
	{
		if (myPins[aPinToFetchFrom].myPinType == SPin::PinTypeInOut::PinTypeInOut_IN)
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
				
				CGraphManager::ShowFlow(links[0]->myLinkID);
				links[0]->myLink->FetchData(outType, someData, outSize, pinIndex);
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
		if (myPins[aPinToFetchFrom].myPinType == SPin::PinTypeInOut::PinTypeInOut_IN)
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
				links[0]->myLink->FetchData(outType, someData, outSize, indexInVector);
				return;
			}
		}
	}

	SPin& dataPin = myPins[aPinToFetchFrom];
	// We have the data, set ut to the return values
	if (dataPin.myVariableType == SPin::PinType::Float)
	{
		outSize = dataPin.myData != nullptr ? sizeof(float) : 0;
	}
	else if (dataPin.myVariableType == SPin::PinType::Int)
	{
		outSize = dataPin.myData != nullptr ? sizeof(int) : 0;
	}
	else if (dataPin.myVariableType == SPin::PinType::Bool)
	{
		outSize = dataPin.myData != nullptr ? sizeof(bool) : 0;
	}
	else if (dataPin.myVariableType == SPin::PinType::String)
	{
		outSize = dataPin.myData != nullptr ? strlen(static_cast<char*>(dataPin.myData)) : 0;
	}
	
	someData = dataPin.myData;
	outType = dataPin.myVariableType;
}

