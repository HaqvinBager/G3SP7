#pragma once
#include "Window.h"

class CGameObject;
namespace ImGuiWindow
{
	namespace AssetCustomizationWindow
	{
		constexpr int JSONNameBufferSize = 256;
	}

	class CTexturePath
	{
	public:
		CTexturePath(): myHasChanged(true), myPath(""){}
		~CTexturePath() {}

		void UpdatePath(const std::string& aPath)
		{
			myPath = aPath;
			myHasChanged = true;
		}

		const std::string& GetPath()
		{
			myHasChanged = false;
			return myPath;
		}

		inline const bool HasChanged()
		{
			return myHasChanged;
		}

	private:
		bool myHasChanged = false;
		std::string myPath;
	};

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
		void LoadAsset();
		void LoadCustomizationFile();
		void SaveCustomizationFile();
		void SaveOverwriteFile();
		void TextureBrowser();

		bool GetPathsByExtension(const std::string& aPath, const std::string& aFileExtesion,std::vector<SAssetInfo>& anOutVector, const bool aShouldClearVector = true);
		bool GetPathsByExtension(const std::string& aPath, const std::string& aFileExtesion,std::vector<std::string>& anOutVector, const bool aShouldClearVector = true);

		void SaveJSON(const std::string& aCustomPath = "");

	private:
		float myPrimaryTint[4];
		float mySecondaryTint[4];
		float myTertiaryTint[4];
		float myAccentTint[4];
		float myEmissiveTint[4];
		std::array<CTexturePath, 4> myTexturePaths;


		CGameObject* myGameObject;
		SAssetInfo mySelectedFBX;
		SAssetInfo mySelectedJSON;
		char myJSONFileName[AssetCustomizationWindow::JSONNameBufferSize];
		std::vector<SAssetInfo> myFBXAssetPaths;
		std::vector<SAssetInfo> myJSONPaths;
		std::vector<SAssetInfo> myTexturePathsInFolder;

		bool myShowLoadAsset;
		bool myShowLoadCustomizationFile;
		bool myShowSaveCustomizationFile;
		bool myShowOverwriteCustomizationFile;
		bool myShowTextureBrowser;
		bool myReplaceFBX;
		short myCurrentTextureSlot;
	};
}

