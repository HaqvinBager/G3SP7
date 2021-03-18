#pragma once
#include "rapidjson/document.h"
#include "rapidjson/istreamwrapper.h"
#include "rapidjson/writer.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/filewritestream.h"
#include <fstream>
#include <filesystem>

//typedef rapidjson::GenericObject<false, rapidjson::Value>& RapidObject;

using rapidjson::IStreamWrapper;
using rapidjson::Document;
using rapidjson::FileWriteStream;
using rapidjson::Writer;

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
	void SwapValues(const char* aFirstKey, const char* aSecondKey);

	template<class T>
	static void WriteGenericValue(const char* aFilePath, const char* aKey, T aValue);
	
	static void WriteString(const char* aFilePath, const char* aKey, const std::string& aValue);
};

template<class T>
inline void CJsonWriter::WriteGenericValue(const char* aFilePath, const char* aKey, T aValue)
{
	std::ifstream inputStream(aFilePath);
	IStreamWrapper inputWrapper(inputStream);
	Document document;
	document.ParseStream(inputWrapper);
	inputStream.close();

	if (document.HasParseError())
	{
		std::ofstream of(aFilePath);
		ENGINE_BOOL_POPUP(of.good(), "Could not create file: %s", aFilePath);
		of.put('{');
		of.put('}');
		of.close();

		inputStream.open(aFilePath);
		IStreamWrapper backupInputWrapper(inputStream);
		document.ParseStream(backupInputWrapper);
		inputStream.close();
	}

	FILE* fp;
	fopen_s(&fp, aFilePath, "w");
	char writeBuffer[200];
	FileWriteStream os(fp, writeBuffer, sizeof(writeBuffer));
	Writer<FileWriteStream> writer(os);

	if (!document.HasMember(aKey))
	{
		document.AddMember(rapidjson::GenericStringRef(aKey), aValue, document.GetAllocator());
	}
	else 
	{
		document[aKey] = aValue;
	}

	document.Accept(writer);
	fclose(fp);
}
