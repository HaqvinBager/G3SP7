#pragma once
#include "rapidjson/document.h"
#include "rapidjson/istreamwrapper.h"
#include <fstream>
//#include <string>
#include <unordered_map>
#include <vector>

struct SDirectory {
	std::string myFileName;
	std::vector<std::string> myFiles;
};


class CJsonReader
{
public:
	static CJsonReader* Get();
	rapidjson::Document LoadDocument(const std::string& json_path);
private:
	CJsonReader() {}
	~CJsonReader() {}
	static CJsonReader* ourInstance;

public:
	static bool IsValid(const rapidjson::Document& aDoc, const std::vector<std::string>& someMembers);

	void Init();
	const std::string& GetAssetPath(const int anAssetID) const;


private:
	std::vector<SDirectory> myDirectories;
	std::unordered_map<int, std::string> myModelAssetMap;


};