#include "stdafx.h"
#include "JsonReader.h"
#include <filesystem>
#include "ModelFactory.h"

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

	for (const auto& member : someMembers)
	{
		if (!aDoc.HasMember(member.c_str()))
			return false;
	}

	return true;
}

bool CJsonReader::HasParseError(const rapidjson::Document& aDoc)
{
	return aDoc.HasParseError();
}

void CJsonReader::InitFromGenerated()
{
	const auto& doc = LoadDocument(ASSETPATH("Assets/Generated/Resource_Assets.json"));
	if (!IsValid(doc, { "models", "vertexColors" }))
		return;

	for (const auto& model : doc.GetObjectW()["models"].GetArray())
	{
		myPathsMap[model["id"].GetInt()] = model["path"].GetString();
	}

	for (const auto& vertexColor : doc.GetObjectW()["vertexColors"].GetArray())
	{
		myPathsMap[vertexColor["id"].GetInt()] = vertexColor["path"].GetString();
	}

	for (const auto& path : myPathsMap)
	{
		std::string substr = path.second.substr(path.second.size() - 4, 4);
		if (substr == ".fbx" && path.second.find("Animat") == std::string::npos)
		{
			CModelFactory::GetInstance()->GetModel(ASSETPATH(path.second));
		}
	}

}

const bool CJsonReader::HasAssetPath(const int anAssetID) const
{
	return myPathsMap.find(anAssetID) != myPathsMap.end();
}
const std::string& CJsonReader::GetAssetPath(const int anAssetID) const
{
	return myPathsMap.at(anAssetID);
}
