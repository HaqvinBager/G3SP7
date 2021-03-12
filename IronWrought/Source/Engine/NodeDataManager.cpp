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
}

//void CNodeDataManager::SetData(const std::string aNodeDataKey, EDataType aDataType, void* aValue)
//{
//	switch (aDataType)
//	{
//	case CNodeDataManager::EDataType::EFloat:
//	{
//		float inputValue = *(static_cast<float*>(aValue));
//		myNodeDataMap[aNodeDataKey] = &inputValue;
//	}
//	break;
//	case CNodeDataManager::EDataType::EInt:
//	{
//		int inputValue = *(static_cast<int*>(aValue));
//		myNodeDataMap[aNodeDataKey] = &inputValue;
//	}
//	break;
//	case CNodeDataManager::EDataType::EBool:
//	{
//		bool inputValue = *(static_cast<bool*>(aValue));
//		myNodeDataMap[aNodeDataKey] = &inputValue;
//	}
//	break;
//	case CNodeDataManager::EDataType::EStart:
//	{
//		bool inputValue = *(static_cast<bool*>(aValue));
//		myNodeDataMap[aNodeDataKey] = &inputValue;
//	}
//	break;
//	}
//}

//template <typename T>
//T CNodeDataManager::GetData(const std::string aNodeDataKey, EDataType /*aDataType*/)
//{
//
//	return (reinterpret_cast<T>(myNodeDataMap[aNodeDataKey]));
//
//	/*switch (aDataType)
//	{
//	case CNodeDataManager::EDataType::EFloat:
//	{
//		return (reinterpret_cast<float*>(myNodeDataMap[aNodeDataKey]));
//	}
//	break;
//	case CNodeDataManager::EDataType::EInt:
//	{
//		return (reinterpret_cast<int*>(myNodeDataMap[aNodeDataKey]));
//	}
//	break;
//	case CNodeDataManager::EDataType::EBool:
//	{
//		return (reinterpret_cast<bool*>(myNodeDataMap[aNodeDataKey]));
//	}
//	break;
//	case CNodeDataManager::EDataType::EStart:
//	{
//		return (reinterpret_cast<bool*>(myNodeDataMap[aNodeDataKey]));
//	}
//	break;
//	default:
//		return NULL;
//	}*/
//}

void CNodeDataManager::SaveDataTypesToJson()
{
	rapidjson::StringBuffer s;
	rapidjson::PrettyWriter<rapidjson::StringBuffer> writer1(s);

	writer1.StartObject();
	writer1.Key("Custom Data");
	writer1.StartArray();
	for (auto& data : myNodeDataMap)
	{
		writer1.StartObject();
		writer1.Key("Data key");
		writer1.String(data.first.c_str());
		
		switch (myNodeDataTypeMap[data.first])
		{
		case CNodeDataManager::EDataType::EFloat:
			writer1.Key("Type");
			writer1.String("Float");
			break;
		case CNodeDataManager::EDataType::EInt:
			writer1.Key("Type");
			writer1.String("Int");
			break;
		case CNodeDataManager::EDataType::EBool:
			writer1.Key("Type");
			writer1.String("Bool");
			break;
		case CNodeDataManager::EDataType::EStart:
			writer1.Key("Type");
			writer1.String("Start");
			break;
		case CNodeDataManager::EDataType::EVector3:
			writer1.Key("Type");
			writer1.String("Vector 3");
			break;
		default:
			break;
		}
		writer1.EndObject();
	}

	/*for (auto& intData : myNodeIntDataMap)
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

	for (auto& boolData : myNodeStartDataMap)
	{
		writer1.StartObject();
		writer1.Key("Data key");
		writer1.String(boolData.first.c_str());

		writer1.Key("Type");
		writer1.String("Start");
		writer1.EndObject();
	}*/
	writer1.EndArray();
	writer1.EndObject();

	std::ofstream of("Imgui/NodeScripts/CustomDataNodes.json");
	of << s.GetString();
}