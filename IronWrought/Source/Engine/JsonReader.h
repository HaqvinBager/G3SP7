#pragma once
#include "rapidjson/document.h"
#include "rapidjson/istreamwrapper.h"
#include <fstream>

#include <string>
#include <filesystem>

class CJsonReader
{
public:
	static CJsonReader* Get();

	/*static*/ rapidjson::Document LoadDocument(const std::string& json_path);

	static std::vector<std::string> GetFilePathsInFolder(const std::string& aFolder, const std::string& aContains = "");

private:
	CJsonReader() {}
	~CJsonReader() {}
	static CJsonReader* ourInstance;
};