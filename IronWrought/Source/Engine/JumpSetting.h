#pragma once
#include "Window.h"
class CPlayerControllerComponent;
class CGameObject;
namespace ImGuiWindow {
	class CJumpSetting : public CWindow
	{
	public:

		CJumpSetting(const char* aName);
		~CJumpSetting();

		void OnEnable() override;
		void OnInspectorGUI() override;
		void OnDisable() override;

	private:

		float myJumpHeight;
	};
}