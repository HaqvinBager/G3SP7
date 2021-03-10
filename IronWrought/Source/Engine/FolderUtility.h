#pragma once
#include <filesystem>
#include "EngineException.h"
class CFolderUtility
{
public:

	//static std::string GetFileNamesInFolder(const std::string aFolder) {

	//	std::string filePath;
	//	for (const auto& file : std::filesystem::directory_iterator(aFolder)) {
	//		if (file.path().extension().string() == ".meta")
	//			continue;

	//		filePath = aFolder;

	//	}
	//	return filePath;
	//}


	static std::vector<std::string> GetFileNamesInFolder(const std::string& aFolder, const std::string& anExtension = "", const std::string& aContains = "")
	{

		std::vector<std::string> filePaths;

		if (!std::filesystem::exists(aFolder))
			return std::vector<std::string>();

		ENGINE_BOOL_POPUP(std::filesystem::exists(aFolder), "Could not find Folder %s", aFolder.c_str());

		for (const auto& file : std::filesystem::directory_iterator(aFolder)) {
			if (file.path().extension().string() == ".meta")
				continue;

			if (anExtension.size() > 0) //If the user has asked for a certain Extension
			{
				if (aContains.size() > 0)
				{
					if (file.path().string().find(aContains) != std::string::npos)
					{
						filePaths.emplace_back(file.path().filename().string());
					}
				}
				else
				{
					if (file.path().string().find(aContains) != std::string::npos)
					{
						filePaths.emplace_back(file.path().filename().string());
					}
				}
			}
			else if (aContains.size() > 0) 	//If the user has asked for the files to contain some key word
			{

				if (file.path().string().find(aContains) != std::string::npos)
				{
					filePaths.emplace_back(file.path().filename().string());
				}
			}
			else  //When the user has only asked for ALL the files inside this folder.
			{
				filePaths.emplace_back(file.path().filename().string());
			}
		}
		return filePaths;
	}
};