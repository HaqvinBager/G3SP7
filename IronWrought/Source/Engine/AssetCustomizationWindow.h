#pragma once
#include "Window.h"

#include "ModelComponent.h"

class CGameObject;
namespace ImGuiWindow
{
	namespace AssetCustomizationWindow
	{
		constexpr int JSONNameBufferSize = 256;
	}

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

		void ClearAll();
		void ClearTexture(const int& anIndex);

		inline void ImGuiSpacing(const short& aNrOf);

	private:
		std::array<DirectX::SimpleMath::Vector4, NUMBER_OF_TINT_SLOTS> myTints;
		//float myPrimaryTint[4];
		//float mySecondaryTint[4];
		//float myTertiaryTint[4];
		//float myAccentTint[4];
		/*float myEmissiveTint[4];*/
		DirectX::SimpleMath::Vector4 myEmissive;
		std::array<std::string, NUMBER_OF_TINT_SLOTS> myCurrentTextures;


		CGameObject* myGameObject;
		SAssetInfo mySelectedFBX;
		SAssetInfo mySelectedJSON;
		char myJSONFileName[AssetCustomizationWindow::JSONNameBufferSize];
		std::vector<SAssetInfo> myFBXAssetPaths;
		std::vector<SAssetInfo> myJSONPaths;
		std::vector<SAssetInfo> myTexturePaths;

		bool myShowLoadAsset;
		bool myShowLoadCustomizationFile;
		bool myShowSaveCustomizationFile;
		bool myShowOverwriteCustomizationFile;
		bool myShowTextureBrowser;
		bool myReplaceFBX;
		short myCurrentTextureSlot;
	};
}

