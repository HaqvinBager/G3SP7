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

bool CJsonReader::IsValid(const rapidjson::Document& aDoc, const std::vector<std::string>& someMembers)
{
	if (aDoc.HasParseError())
		return false;

	for (const auto& member : someMembers) {
		if (!aDoc.HasMember(member.c_str()))
			return false;
	}

	return true;
}

bool CJsonReader::HasParseError(const rapidjson::Document& aDoc)
{
	return aDoc.HasParseError();
}

void CJsonReader::Init()
{
	const auto& doc = LoadDocument(ASSETPATH("Assets/Generated/Resource_Assets.json"));
	if (!IsValid(doc, { "models" }))
		return;

	for (const auto& model : doc.GetObjectW()["models"].GetArray()) {
		myModelAssetMap[model["id"].GetInt()] = model["path"].GetString();
	}
}

const std::string& CJsonReader::GetAssetPath(const int anAssetID) const
{
	return myModelAssetMap.at(anAssetID);
}
