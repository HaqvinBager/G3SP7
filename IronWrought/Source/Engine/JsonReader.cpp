#include "stdafx.h"
#include "JsonReader.h"
#include <filesystem>

namespace fs = std::filesystem;

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

void CJsonReader::Init()
{
	const auto& doc = LoadDocument(ASSETPATH("Assets/Generated/Resource_Assets.json"));
	for (const auto& model : doc.GetObjectW()["models"].GetArray()) {
		//const int id = model["id"].GetInt();
		//const std::string path = model["path"].GetString();
		myModelAssetMap[model["id"].GetInt()] = model["path"].GetString();
	}
}

const std::string& CJsonReader::GetAssetPath(const int anAssetID) const
{
	return myModelAssetMap.at(anAssetID);
}
