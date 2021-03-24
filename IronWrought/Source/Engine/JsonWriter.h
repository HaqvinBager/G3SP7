#pragma once
#include "rapidjson/document.h"
#include "rapidjson/istreamwrapper.h"
#include "rapidjson/writer.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/filewritestream.h"
#include <fstream>
#include <filesystem>

//using RapidObject = const rapidjson::Value::Object&;
using rapidjson::IStreamWrapper;
using rapidjson::Document;
using rapidjson::FileWriteStream;
using rapidjson::Writer;

template<unsigned int BUFFERSIZE = 200>
struct SJsonFile {

	SJsonFile(const char* aFilePath)
	{
		Open(aFilePath);
	}

	void Open(const char* aFilePath)
	{
		fopen_s(&fp, aFilePath, "w");
	}

	void Close()
	{
		fclose(fp);
		ZeroMemory(writeBuffer, BUFFERSIZE);
	}


	FILE* fp;
	char writeBuffer[BUFFERSIZE];
};

class CJsonWriter
{
public:
	static CJsonWriter* Get();

private:
	CJsonWriter() { }
	~CJsonWriter() { }
	static CJsonWriter* ourInstance;

public:
	void Init();

public:
	//void SwapValues(const char* aFirstKey, const char* aSecondKey);

	/*template<class T>
	void WriteGenericValues(const char* aFilePath, const char* aKey, const std::vector<T>& someValues);*/

	template<class T>
	void WriteGenericValue(const char* aFilePath, const char* aKey, T aValue);	

	template<class T>
	void WriteGenericValue(const char* aFilePath, const std::vector<std::string>& someKeys, const T& aValue);

	void WriteString(const char* aFilePath, const char* aKey, const std::string& aValue);

	rapidjson::Document GetCreateDocument(const char* aFilePath);

};

template<class T>
void CJsonWriter::WriteGenericValue(const char* aFilePath, const char* aKey, T aValue)
{
	Document document = GetCreateDocument(aFilePath);
	SJsonFile file(aFilePath);
	
	FileWriteStream os(file.fp, file.writeBuffer, sizeof(file.writeBuffer));
	Writer<FileWriteStream> writer(os);

	if (!document.HasMember(aKey))	
		document.AddMember(rapidjson::GenericStringRef(aKey), aValue, document.GetAllocator());
	else 
		document[aKey] = aValue;
	
	document.Accept(writer);
	file.Close();
}

template<class T>
 void CJsonWriter::WriteGenericValue(const char* aFilePath, const std::vector<std::string>& someKeys, const T& aValue)
{
	 Document document = GetCreateDocument(aFilePath);
	 SJsonFile file(aFilePath);

	 FileWriteStream os(file.fp, file.writeBuffer, sizeof(file.writeBuffer));
	 Writer<FileWriteStream> writer(os);

	 auto propertyArray = document[someKeys[0].c_str()].GetArray();

	 auto index = std::stoi(someKeys[1].c_str());
	 auto scopedObject = propertyArray[index].GetObjectW();

	 for (unsigned int i = 2; i < someKeys.size() - 1; ++i)
	 {
		 if (scopedObject[someKeys[i].c_str()].IsArray())
		 {
			 index = std::stoi(someKeys[i + 1]);
			 scopedObject = scopedObject[someKeys[i].c_str()].GetArray()[index].GetObjectW();
			 i++;
			 continue;
		 }		 
		 scopedObject = scopedObject[someKeys[i].c_str()].GetObjectW();
	 }

	 if (!scopedObject.HasMember(someKeys.back().c_str()))
		 scopedObject.AddMember(rapidjson::GenericStringRef(someKeys.back().c_str()), aValue, document.GetAllocator());
	 else
		 scopedObject[someKeys.back().c_str()] = aValue;

	 document.Accept(writer);
	 file.Close();
}


/*std::ifstream inputStream(aFilePath);
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
}*/

/*SJsonFile<200> file;
auto writer = file.OpenWriter(aFilePath);*/

//FILE* fp;
//fopen_s(&fp, aFilePath, "w");
//char writeBuffer[200];

//
//template<class T>
//inline void CJsonWriter::WriteGenericValues(const char* aFilePath, const char* aKey, const std::vector<T>& someValues)
//{
//	std::ifstream inputStream(aFilePath);
//	IStreamWrapper inputWrapper(inputStream);
//	Document document;
//	document.ParseStream(inputWrapper);
//	inputStream.close();
//
//	if (document.HasParseError())
//	{
//		std::ofstream of(aFilePath);
//		ENGINE_BOOL_POPUP(of.good(), "Could not create file: %s", aFilePath);
//		of.put('{');
//		of.put('}');
//		of.close();
//
//		inputStream.open(aFilePath);
//		IStreamWrapper backupInputWrapper(inputStream);
//		document.ParseStream(backupInputWrapper);
//		inputStream.close();
//	}
//
//	FILE* fp;
//	fopen_s(&fp, aFilePath, "w");
//	char writeBuffer[32 * someValues.size()];
//	FileWriteStream os(fp, writeBuffer, sizeof(writeBuffer));
//	Writer<FileWriteStream> writer(os);
//
//}
