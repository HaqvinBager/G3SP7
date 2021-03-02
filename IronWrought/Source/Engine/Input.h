#pragma once
#include "WinInclude.h"
#include <Windowsx.h>
#include <bitset>
#include <memory>
#include <stack>

#define INPUT Input::GetInstance()

class Input {
public:

	static Input* GetInstance();

	enum class EMouseButton {
		Left = 0,
		Right = 1,
		Middle = 2,
		Mouse4 = 3,
		Mouse5 = 4

	};//regular enum was easier to use.

	Input();

	bool UpdateEvents(UINT message, WPARAM wParam, LPARAM lParam);
	void Update();


	bool MoveLeft();
	bool MoveRight();
	bool MoveUp();
	bool MoveDown();

	bool IsKeyDown(WPARAM wParam);
	bool IsKeyPressed(WPARAM wParam);
	bool IsKeyReleased(WPARAM wParam);

	int MouseX();//x coordiantes in app window
	int MouseY();//y coordiantes in app window
	int MouseScreenX();
	int MouseScreenY();
	int MouseDeltaX();
	int MouseDeltaY();
	int MouseRawDeltaX();
	int MouseRawDeltaY();
	int MouseWheel();
	void SetMouseScreenPosition(int x, int y);
	bool IsMouseDown(EMouseButton aMouseButton);
	bool IsMousePressed(EMouseButton aMouseButton);
	bool IsMouseReleased(EMouseButton aMouseButton);

private:


private:
	std::bitset<5> myMouseButtonLast;
	std::bitset<5> myMouseButton;

	std::bitset<256> myKeyDownLast;
	std::bitset<256> myKeyDown;

	int myMouseX;
	int myMouseY;
	int myMouseScreenX;
	int myMouseScreenY;
	int myMouseXLast;
	int myMouseYLast;
	int myMouseRawXLast;
	int myMouseRawYLast;
	int myMouseWheel;//positive = away from user, negative = towards user
};