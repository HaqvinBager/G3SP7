#pragma once
#include "rapidjson/document.h"
#include "rapidjson/istreamwrapper.h"
#include <fstream>

class CJsonWriter
{
public:
	static CJsonWriter* Get();

private:
	CJsonWriter() {}
	~CJsonWriter() {}
	static CJsonWriter* ourInstance;

public:
	void Init();

public:
	void SwapValues(rapidjson::GenericObject<false, rapidjson::Value>& anObject, const char* aFirstKey, const char* aSecondKey);
	void WriteString(rapidjson::GenericObject<false, rapidjson::Value>&, const char* aKey, const std::string& aValue);
	void WriteFloat(rapidjson::GenericObject<false, rapidjson::Value>&, const char* aKey, float aValue);
	void WriteInt(rapidjson::GenericObject<false, rapidjson::Value>&, const char* aKey, int aValue);
	void WriteBool(rapidjson::GenericObject<false, rapidjson::Value>&, const char* aKey, bool aValue);
};

