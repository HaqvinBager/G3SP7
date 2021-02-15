#pragma once
#include <filesystem>

class CFolderUtility
{
public:

	static std::string GetFilePathsInFolder(const std::string& aFolder) {

		std::string filePath;
		for (const auto& file : std::filesystem::directory_iterator(aFolder)) {
			if (file.path().extension().string() == ".meta")
				continue;
			
				filePath = aFolder;
			
		}
		return filePath;
	}


	static std::vector<std::string> GetFilePathsInFolder(const std::string& aFolder, const std::string& aExtansion, const std::string& aContains = "") {

		std::vector<std::string> filePaths;
		for (const auto& file : std::filesystem::directory_iterator(aFolder)) {
			if (file.path().extension().string() == ".meta")
				continue;

			if (file.path().extension().string() == aExtansion) {
				if (aContains.size() > 0) {
					if (file.path().string().find(aContains) != std::string::npos) {
						filePaths.emplace_back(file.path().filename().string());
					}
				}
				else {
					if (file.path().string().find(aContains) != std::string::npos) {
						filePaths.emplace_back(file.path().filename().string());
					}
				}
			}
		}
		return filePaths;
	}
};