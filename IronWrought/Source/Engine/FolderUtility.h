#pragma once
#include <filesystem>

class CFolderUtility
{
public:

	static std::vector<std::string> GetFilePathsInFolder(const std::string& aFolder) {

		std::vector<std::string> filePaths;
		for (const auto& file : std::filesystem::directory_iterator(aFolder)) {
			if (file.path().extension().string() == ".meta")
				continue;
			
				filePaths.emplace_back(file.path().filename().string());
			
		}
		return filePaths;
	}


	static std::vector<std::string> GetFilePathsInFolder(const std::string& aFolder, const std::string& aExtansion) {

		std::vector<std::string> filePaths;
		for (const auto& file : std::filesystem::directory_iterator(aFolder)) {
			if (file.path().extension().string() == ".meta")
				continue;
			if (file.path().extension().string() == aExtansion) {
				filePaths.emplace_back(file.path().filename().string());
			}
		}
		return filePaths;
	}
};