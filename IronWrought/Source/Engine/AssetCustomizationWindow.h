#pragma once
#include "Window.h"

class CGameObject;
namespace ImGuiWindow
{
	class CAssetCustomizationWindow : public CWindow
	{
	public:
		CAssetCustomizationWindow(const char* aName);
		~CAssetCustomizationWindow() override;
		void OnEnable() override;
		void OnInspectorGUI() override;
		void OnDisable() override;

	private:
		struct SAssetInfo
		{
			std::string myPath;
			std::string myDisplayName;
		};
		void OpenAsset();
		void OpenCustomizationFile();
		void SaveCustomizationFile();

		bool GetPathsByExtension(const std::string& aPath, const std::string& aFileExtesion,std::vector<SAssetInfo>& anOutVector, const bool aShouldClearVector = true);

	private:
		float myPrimaryTint[3];
		float mySecondaryTint[3];
		float myTertiaryTint[3];
		float myAccentTintTint[3];

		CGameObject* myGameObject;
		std::vector<SAssetInfo> myFBXAssetPaths;

		bool myShowOpenAssetWindow;
		bool myShowOpenCustomizationWindow;
		bool myShowSaveCustomizationWindow;
	};
}

