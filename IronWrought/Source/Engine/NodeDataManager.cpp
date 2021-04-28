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
#include "Scene.h"
#include "Engine.h"

CNodeDataManager* CNodeDataManager::ourInstance = nullptr;

CNodeDataManager::~CNodeDataManager()
{
	ClearStoredData();
}

void CNodeDataManager::ClearStoredData()
{
	SaveDataTypesToJson();
	for (auto& data : myNodeData)
	{
		delete data.myData;
		data.myData = nullptr;
	}
	myNodeData.clear();
}

void CNodeDataManager::SaveDataTypesToJson()
{
	rapidjson::StringBuffer s;
	rapidjson::PrettyWriter<rapidjson::StringBuffer> writer1(s);

	writer1.StartObject();
	writer1.Key("Custom Data");
	writer1.StartArray();
	for (auto& data : myNodeData)
	{
		if (data.myShouldSave)
		{
			writer1.StartObject();
			writer1.Key("Data key");
			writer1.String(data.myNodeTypeName.c_str());
			writer1.Key("Type");
			switch (data.myDataType)
			{
			case CNodeDataManager::EDataType::EFloat:
				writer1.String("Float");
				break;
			case CNodeDataManager::EDataType::EInt:
				writer1.String("Int");
				break;
			case CNodeDataManager::EDataType::EBool:
				writer1.String("Bool");
				break;
			case CNodeDataManager::EDataType::EStart:
				writer1.String("Start");
				break;
			case CNodeDataManager::EDataType::EVector3:
				writer1.String("Vector 3");
				break;
			default:
				break;
			}
			writer1.EndObject();
		}
	}
	writer1.EndArray();
	writer1.EndObject();

	std::ofstream of(myCurrentFolderPath + "CustomDataNodes.json");
	of << s.GetString();
}