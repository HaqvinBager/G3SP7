#include "stdafx.h"
#include "JsonWriter.h"

using namespace rapidjson;

CJsonWriter* CJsonWriter::ourInstance = nullptr;
CJsonWriter* CJsonWriter::Get()
{
    if (ourInstance == nullptr)
        ourInstance = new CJsonWriter();
    return ourInstance;
}


void CJsonWriter::Init()
{
}

void CJsonWriter::WriteString(const char* aFilePath, const char* aKey, const std::string& aValue)
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

	document[aKey] = rapidjson::GenericStringRef(aValue.c_str());

	document.Accept(writer);
	fclose(fp);
}

rapidjson::Document CJsonWriter::GetCreateDocument(const char* aFilePath)
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
	return document;
}
