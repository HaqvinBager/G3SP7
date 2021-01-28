#pragma once
#include <fstream>
#include "../Includes/rapidjson/document.h"
#include "../Includes/rapidjson/istreamwrapper.h"

#include <string>
#include <filesystem>

class CJsonReader
{
public:
	static rapidjson::Document LoadDocument(const std::string json_path)
	{
		std::ifstream stream(json_path);
		rapidjson::IStreamWrapper wrapper(stream);
		rapidjson::Document document;
		document.ParseStream(wrapper);
		return document;
	}

	static std::vector<std::string> GetFilePathsInFolder(const std::string& aFolder)
	{
		std::vector<std::string> filePaths;
		for (const auto& file : std::filesystem::directory_iterator(aFolder))
		{
			if (file.path().extension().string() == ".meta")
				continue;

			filePaths.emplace_back(file.path().filename().string());
		}

		return filePaths;
	}

};