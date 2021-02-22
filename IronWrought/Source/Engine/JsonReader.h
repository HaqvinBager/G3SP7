#pragma once
#include "rapidjson/document.h"
#include "rapidjson/istreamwrapper.h"
#include <fstream>
//#include <string>

class CJsonReader
{
public:
	static CJsonReader* Get();
	rapidjson::Document LoadDocument(const std::string& json_path);
private:
	CJsonReader() {}
	~CJsonReader() {}
	static CJsonReader* ourInstance;
};