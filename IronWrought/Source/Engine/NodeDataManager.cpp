#include "stdafx.h"
#include "NodeDataManager.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/prettywriter.h"
#include <fstream>
#include <iostream>
#include <sstream>

CNodeDataManager* CNodeDataManager::ourInstance = nullptr;

CNodeDataManager::~CNodeDataManager()
{
	ClearStoredData();
}

void CNodeDataManager::ClearStoredData()
{
	myNodeFloatDataMap.clear();
	myNodeIntDataMap.clear();
	myNodeBoolDataMap.clear();
}

void CNodeDataManager::SetData(const std::string aNodeDataKey, EDataType aDataType, void* aValue)
{
	switch (aDataType)
	{
	case CNodeDataManager::EDataType::EFloat:
	{
		float inputValue = *(static_cast<float*>(aValue));
		myNodeFloatDataMap[aNodeDataKey] = inputValue;
	}
	break;
	case CNodeDataManager::EDataType::EInt:
	{
		int inputValue = *(static_cast<int*>(aValue));
		myNodeIntDataMap[aNodeDataKey] = inputValue;
	}
	break;
	case CNodeDataManager::EDataType::EBool:
	{
		bool inputValue = *(static_cast<bool*>(aValue));
		myNodeBoolDataMap[aNodeDataKey] = inputValue;
	}
	break;
	}
}

void* CNodeDataManager::GetData(const std::string aNodeDataKey, EDataType aDataType)
{
	switch (aDataType)
	{
	case CNodeDataManager::EDataType::EFloat:
	{
		return &myNodeFloatDataMap[aNodeDataKey];
	}
	case CNodeDataManager::EDataType::EInt:
	{
		return &myNodeIntDataMap[aNodeDataKey];
	}
	case CNodeDataManager::EDataType::EBool:
	{
		return &myNodeBoolDataMap[aNodeDataKey];
	}
	default:
		return NULL;
	}
}

void CNodeDataManager::SaveDataTypesToJson()
{
	rapidjson::StringBuffer s;
	rapidjson::PrettyWriter<rapidjson::StringBuffer> writer1(s);

	writer1.StartObject();
	writer1.Key("Custom Data");
	writer1.StartArray();
	for (auto& floatData : myNodeFloatDataMap)
	{
		writer1.StartObject();
		writer1.Key("Data key");
		writer1.String(floatData.first.c_str());
		
		writer1.Key("Type");
		writer1.String("Float");
		writer1.EndObject();
	}

	for (auto& intData : myNodeIntDataMap)
	{
		writer1.StartObject();
		writer1.Key("Data key");
		writer1.String(intData.first.c_str());

		writer1.Key("Type");
		writer1.String("Int");
		writer1.EndObject();
	}

	for (auto& boolData : myNodeBoolDataMap)
	{
		writer1.StartObject();
		writer1.Key("Data key");
		writer1.String(boolData.first.c_str());

		writer1.Key("Type");
		writer1.String("Bool");
		writer1.EndObject();
	}
	writer1.EndArray();
	writer1.EndObject();

	std::ofstream of("Imgui/NodeScripts/CustomDataNodes.json");
	of << s.GetString();
}