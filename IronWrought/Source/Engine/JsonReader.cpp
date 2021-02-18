#include "stdafx.h"
#include "JsonReader.h"

CJsonReader* CJsonReader::ourInstance = nullptr;

CJsonReader* CJsonReader::Get()
{
	if (ourInstance == nullptr)
		ourInstance = new CJsonReader();

	return ourInstance;
}

rapidjson::Document CJsonReader::LoadDocument(const std::string& json_path)
{
	std::string path;
	path = json_path;
	std::ifstream stream(path);
	rapidjson::IStreamWrapper wrapper(stream);
	rapidjson::Document document;
	document.ParseStream(wrapper);
	return document;
}

std::vector<std::string> CJsonReader::GetFilePathsInFolder(const std::string& aFolder, const std::string& aContains)
{
	std::vector<std::string> filePaths;
	if (std::filesystem::exists(aFolder)) {
		for (const auto& file : std::filesystem::directory_iterator(aFolder))
		{
			if (file.path().extension().string() == ".meta")
				continue;

			if (aContains.size() > 0) {
				if (file.path().filename().string().find(aContains) != std::string::npos) {
					filePaths.emplace_back(file.path().filename().string());
				}
			}
			else {
				filePaths.emplace_back(file.path().filename().string());

			}
		}
	}
	return filePaths;
}
