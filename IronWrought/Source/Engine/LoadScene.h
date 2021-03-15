#pragma once
#include "Window.h"

namespace IronWroughtImGui {

	class CLoadScene : public CWindow
	{
	public:
		CLoadScene(const char* aMenuName, const bool aIsMenuChild);
		~CLoadScene() override;

		void OnEnable() override;
		void OnInspectorGUI() override;
		bool OnMainMenuGUI() override;
		void OnDisable() override;

	private:
		enum class EState {
			DropDownMenu,
		} myState;


		int mySceneIndex;
		std::vector<std::string> myScenes;
	};
}

