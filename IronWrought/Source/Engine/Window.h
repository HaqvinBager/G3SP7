#pragma once
#include <imgui.h>

namespace IronWroughtImGui {
	class CWindow
	{
	public:
		virtual ~CWindow() { }
		CWindow(const char* aWindowName, const bool aIsMainMenuBarChild = false) 
			: myName(aWindowName)
			, myIsEnabled(false)
			, myIsMainMenuBarChild(aIsMainMenuBarChild)
			{ }
		inline const char* Name() { return myName; }
		inline void Enable(const bool aEnable) { myIsEnabled = aEnable; }
		inline const bool Enable() const { return myIsEnabled; }
		inline const bool MainMenuBarChild() const { return myIsMainMenuBarChild; }
	
	public:
		virtual void OnEnable() = 0;
		virtual void OnInspectorGUI() = 0;
		virtual void OnDisable() = 0;

		virtual bool OnMainMenuGUI() { 
			if (ImGui::Button(Name())) {
				if (!Enable())
					OnEnable();
				else
					OnDisable();
				Enable(!Enable());
			}

			return false;
		}


	protected:
		bool* Open() { return &myIsEnabled; }


	private:
		const char* myName;
		bool myIsEnabled;
		bool myIsMainMenuBarChild;
	};
}