#include "stdafx.h"
#include "InputMapper.h"
#include "Input.h"

CInputMapper* CInputMapper::ourInstance = nullptr;

CInputMapper* CInputMapper::GetInstance()
{
	return ourInstance;
}

CInputMapper::CInputMapper() : myInput(Input::GetInstance())
{
	ourInstance = this;
}

CInputMapper::~CInputMapper()
{
}

bool CInputMapper::Init()
{
	MapEvent(IInputObserver::EInputAction::MouseLeftPressed, IInputObserver::EInputEvent::MoveClick);
	MapEvent(IInputObserver::EInputAction::MouseLeftDown, IInputObserver::EInputEvent::MoveDown);
	MapEvent(IInputObserver::EInputAction::MouseRight, IInputObserver::EInputEvent::AttackClick);
	MapEvent(IInputObserver::EInputAction::MouseMiddle, IInputObserver::EInputEvent::MiddleMouseMove);
	MapEvent(IInputObserver::EInputAction::KeyShiftDown, IInputObserver::EInputEvent::StandStill);
	MapEvent(IInputObserver::EInputAction::KeyShiftRelease, IInputObserver::EInputEvent::Moving);
	MapEvent(IInputObserver::EInputAction::KeyW, IInputObserver::EInputEvent::MoveForward);
	MapEvent(IInputObserver::EInputAction::KeyA, IInputObserver::EInputEvent::MoveLeft);
	MapEvent(IInputObserver::EInputAction::KeyS, IInputObserver::EInputEvent::MoveBackward);
	MapEvent(IInputObserver::EInputAction::KeyD, IInputObserver::EInputEvent::MoveRight);
	MapEvent(IInputObserver::EInputAction::KeyEscape, IInputObserver::EInputEvent::PauseGame);
	MapEvent(IInputObserver::EInputAction::KeySpace, IInputObserver::EInputEvent::Jump);
	MapEvent(IInputObserver::EInputAction::CTRL, IInputObserver::EInputEvent::Crouch);


	if (this == nullptr)
		return false;
	else
		return true;
}

void CInputMapper::RunEvent(const IInputObserver::EInputEvent aOutputEvent)
{
	for (int i = 0; i < myObservers[aOutputEvent].size(); ++i)
	{
		myObservers[aOutputEvent][i]->ReceiveEvent(aOutputEvent);
	}
}

void CInputMapper::TranslateActionToEvent(const IInputObserver::EInputAction aAction)
{
	const auto eventIterator = myEvents.find(aAction);
	if (eventIterator != myEvents.end())
	{
		RunEvent(myEvents[aAction]);
	}
}

void CInputMapper::UpdateKeyboardInput()
{
	if (myInput->IsKeyPressed(VK_SPACE))
	{
		TranslateActionToEvent(IInputObserver::EInputAction::KeySpace);
	}

	if (myInput->IsKeyPressed(VK_ESCAPE))
	{
		TranslateActionToEvent(IInputObserver::EInputAction::KeyEscape);
	}
	
	if (myInput->IsKeyDown(VK_SHIFT))
	{
		TranslateActionToEvent(IInputObserver::EInputAction::KeyShiftDown);
	}
	
	if (myInput->IsKeyReleased(VK_SHIFT))
	{
		TranslateActionToEvent(IInputObserver::EInputAction::KeyShiftRelease);
	}

	if (myInput->IsKeyDown('W'))
	{
		TranslateActionToEvent(IInputObserver::EInputAction::KeyW);
	}
	if (myInput->IsKeyDown('A'))
	{
		TranslateActionToEvent(IInputObserver::EInputAction::KeyA);
	}
	if (myInput->IsKeyDown('S'))
	{
		TranslateActionToEvent(IInputObserver::EInputAction::KeyS);
	}
	if (myInput->IsKeyDown('D'))
	{
		TranslateActionToEvent(IInputObserver::EInputAction::KeyD);
	}
	if (myInput->IsKeyPressed(VK_CONTROL))
	{
		TranslateActionToEvent(IInputObserver::EInputAction::CTRL);
	}
}

void CInputMapper::UpdateMouseInput()
{

	if (myInput->IsMouseDown(Input::EMouseButton::Middle))
	{
		TranslateActionToEvent(IInputObserver::EInputAction::MouseMiddle);
	}
	if (myInput->IsMousePressed(Input::EMouseButton::Left))
	{
		TranslateActionToEvent(IInputObserver::EInputAction::MouseLeftPressed);
	}
	if (myInput->IsMouseDown(Input::EMouseButton::Left))
	{
		TranslateActionToEvent(IInputObserver::EInputAction::MouseLeftDown);
	}
	//else if (myInput->IsMouseDown(Input::EMouseButton::Left))
	//{
	//	TranslateActionToEvent(IInputObserver::EInputAction::MouseLeft);
	//}
	//else if (myInput->IsMouseReleased(Input::EMouseButton::Left))
	//{
	//	TranslateActionToEvent(IInputObserver::EInputAction::MouseLeft);
	//}
	if (myInput->IsMousePressed(Input::EMouseButton::Right))
	{
		TranslateActionToEvent(IInputObserver::EInputAction::MouseRight);
	}
}

void CInputMapper::Update()
{
	UpdateKeyboardInput();
	UpdateMouseInput();
	myInput->Update();
}

void CInputMapper::MapEvent(const IInputObserver::EInputAction aInputEvent, const IInputObserver::EInputEvent aOutputEvent)
{
	myEvents[aInputEvent] = aOutputEvent;
}

bool CInputMapper::AddObserver(const IInputObserver::EInputEvent aEventToListenFor, IInputObserver* aObserver)
{
	ENGINE_ERROR_BOOL_MESSAGE(aObserver, "InputObsever is nullptr!");
	auto it = std::find(myObservers[aEventToListenFor].begin(), myObservers[aEventToListenFor].end(), aObserver);
	if(it == myObservers[aEventToListenFor].end())
		myObservers[aEventToListenFor].emplace_back(aObserver);
	return true;
}

bool CInputMapper::RemoveObserver(const IInputObserver::EInputEvent aEventToListenFor, IInputObserver* aObserver)
{
	ENGINE_ERROR_BOOL_MESSAGE(aObserver, "InputObsever is nullptr!");
	auto it = std::find(myObservers[aEventToListenFor].begin(), myObservers[aEventToListenFor].end(), aObserver);
	if(it != myObservers[aEventToListenFor].end())
		myObservers[aEventToListenFor].erase(it);
	return true;
}

bool CInputMapper::HasObserver(const IInputObserver::EInputEvent aEventToListenFor, IInputObserver* aObserver)
{
	auto it = std::find(myObservers[aEventToListenFor].begin(), myObservers[aEventToListenFor].end(), aObserver);
	return it == myObservers[aEventToListenFor].end();
}

void CInputMapper::ClearObserverList(const IInputObserver::EInputEvent aEventToListenFor)
{
	myObservers[aEventToListenFor].clear();
}
