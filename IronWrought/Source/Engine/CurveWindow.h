#pragma once
#include "Window.h"

namespace IronWroughtImGui {

	//struct VFXSubWindow {
	//	bool myIsOpen;
	//	std::string myName;
	//};

	class CCurveWindow : public CWindow
	{
	public:
		CCurveWindow(const char* aName);
		~CCurveWindow() override;

		void OnEnable()	override;
		void OnInspectorGUI() override;
		void OnDisable() override;
		void SaveToFile();

	private:
		//void Save(const char* aJsonFile, const char* aVariableName, const std::vector<ImVec2>& somePoints);




		std::unordered_map<std::string, std::array<ImVec2, 4>> myPointsMap;
	};
}

