#include "stdafx.h"
#include "Input.h"
#include "imgui.h"

Input* Input::GetInstance()
{
	static Input* input = new Input();
	return input;
}

Input::Input() {
	myMouseX = 0;
	myMouseY = 0;
	myMouseXLast = 0;
	myMouseYLast = 0;
	myMouseRawXLast = 0;
	myMouseRawYLast = 0;
	myMouseWheel = 0;
	myMouseScreenX = 0;
	myMouseScreenY = 0;

	RAWINPUTDEVICE rid;
	rid.usUsagePage = 0x01; // For mouse
	rid.usUsage = 0x02; // For mouse
	rid.dwFlags = 0;
	rid.hwndTarget = nullptr;
	if (RegisterRawInputDevices(&rid, 1, sizeof(rid)) == FALSE)
	{
		ENGINE_BOOL_POPUP(false, "Mouse could not be registered as Raw Input Device")
	}
}

bool Input::UpdateEvents(UINT message, WPARAM wParam, LPARAM lParam) {

	std::vector<char> rawBuffer;

	switch (message) {
	case WM_KEYDOWN:
		myKeyDown[wParam] = true;
		return true;
	case WM_KEYUP:
		myKeyDown[wParam] = false;
		return true;
	case WM_MOUSEMOVE:
		myMouseX = GET_X_LPARAM(lParam);//returns x coordiante
		myMouseY = GET_Y_LPARAM(lParam);//returns y coordinate
		return true;
	case WM_MOUSEWHEEL:
		myMouseWheel += GET_WHEEL_DELTA_WPARAM(wParam);//returns difference in mouse wheel position
		return true;
	case WM_LBUTTONDOWN:
		myMouseButton[(int)EMouseButton::Left] = true;
		return true;
	case WM_LBUTTONUP:
		myMouseButton[(int)EMouseButton::Left] = false;
		return true;
	case WM_RBUTTONDOWN:
		myMouseButton[(int)EMouseButton::Right] = true;
		return true;
	case WM_RBUTTONUP:
		myMouseButton[(int)EMouseButton::Right] = false;
		return true;
	case WM_MBUTTONDOWN:
		myMouseButton[(int)EMouseButton::Middle] = true;
		return true;
	case WM_MBUTTONUP:
		myMouseButton[(int)EMouseButton::Middle] = false;
		return true;
	case WM_XBUTTONDOWN:
		if (GET_XBUTTON_WPARAM(wParam) == XBUTTON1) {
			myMouseButton[(int)EMouseButton::Mouse4] = true;
		}
		else if (GET_XBUTTON_WPARAM(wParam) == XBUTTON2) {
			myMouseButton[(int)EMouseButton::Mouse5] = true;
		}
		return true;
	case WM_XBUTTONUP:
		if (GET_XBUTTON_WPARAM(wParam) == XBUTTON1) {
			myMouseButton[(int)EMouseButton::Mouse4] = false;
		}
		else if (GET_XBUTTON_WPARAM(wParam) == XBUTTON2) {
			myMouseButton[(int)EMouseButton::Mouse5] = false;
		}
		break;
		// Raw Input
	case WM_INPUT:
		UINT size;
		if (GetRawInputData(
			reinterpret_cast<HRAWINPUT>(lParam),
			RID_INPUT,
			nullptr,
			&size,
			sizeof(RAWINPUTHEADER)) == -1)
		{
			// Error if inputdata == -1
			break;
		}
		rawBuffer.resize(size);

		if (GetRawInputData(
			reinterpret_cast<HRAWINPUT>(lParam),
			RID_INPUT,
			rawBuffer.data(),
			&size,
			sizeof(RAWINPUTHEADER)) != size)
		{
			// Probably an error if the size doesn't match up
			break;
		}
		auto& rawInput = reinterpret_cast<const RAWINPUT&>(*rawBuffer.data());
		if (rawInput.header.dwType == RIM_TYPEMOUSE &&
			(rawInput.data.mouse.lLastX != 0 || rawInput.data.mouse.lLastY != 0))
		{
			// Register raw input
			myMouseRawXLast = rawInput.data.mouse.lLastX;
			myMouseRawYLast = rawInput.data.mouse.lLastY;
		}
		break;
	}

	return false;
}

void Input::Update() {
	myKeyDownLast = myKeyDown;

	myMouseXLast = myMouseX;
	myMouseYLast = myMouseY;
	myMouseRawXLast = 0;
	myMouseRawYLast = 0;
	myMouseWheel = 0;
	myMouseButtonLast = myMouseButton;

	POINT point;
	if (GetCursorPos(&point)) {
		myMouseScreenX = point.x;
		myMouseScreenY = point.y;
	}
}

bool Input::MoveLeft() {
	return IsKeyPressed('A') == true || IsKeyPressed(VK_LEFT) == true;
}

bool Input::MoveRight() {
	return IsKeyPressed('D') == true || IsKeyPressed(VK_RIGHT) == true;
}

bool Input::MoveUp() {
	return IsKeyPressed('W') == true || IsKeyPressed(VK_UP) == true;
}

bool Input::MoveDown() {
	return IsKeyPressed('S') == true || IsKeyPressed(VK_DOWN) == true;
}
//const auto& ImguiInput = ImGui::GetIO();

bool Input::IsKeyDown(WPARAM wParam) {
	
	
	//if (ImguiInput.WantCaptureMouse)
	//{
	//	return false;
	//}

	return myKeyDown[wParam];
}

bool Input::IsKeyPressed(WPARAM wParam) {
	return myKeyDown[wParam] && (!myKeyDownLast[wParam]);
}

bool Input::IsKeyReleased(WPARAM wParam) {
	return (!myKeyDown[wParam]) && myKeyDownLast[wParam];
}

int Input::MouseX() {
	return myMouseX;
}

int Input::MouseY() {
	return myMouseY;
}

int Input::MouseScreenX() {
	return myMouseScreenX;
}

int Input::MouseScreenY() {
	return myMouseScreenY;
}

int Input::MouseDeltaX() {
	return (myMouseX - myMouseXLast);
}

int Input::MouseDeltaY() {
	return (myMouseY - myMouseYLast);
}

int Input::MouseRawDeltaX()
{
	return myMouseRawXLast;
}

int Input::MouseRawDeltaY()
{
	return myMouseRawYLast;
}

int Input::MouseWheel() {
	return myMouseWheel;
}

void Input::SetMouseScreenPosition(int x, int y) {
	SetCursorPos(x, y);
}

bool Input::IsMouseDown(EMouseButton aMouseButton) {
	return myMouseButton[(int)aMouseButton];
}

bool Input::IsMousePressed(EMouseButton aMouseButton) {
	return myMouseButton[(int)aMouseButton] && (!myMouseButtonLast[(int)aMouseButton]);
}

bool Input::IsMouseReleased(EMouseButton aMouseButton) {
	return (!myMouseButton[(int)aMouseButton]) && myMouseButtonLast[(int)aMouseButton];
}