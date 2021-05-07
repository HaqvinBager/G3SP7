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
#include "GraphManager.h"
#include "NodeType.h"

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

void CNodeDataManager::RegisterNewDataNode(std::string aName, CGraphManager& aGraphManager)
{
	if (aGraphManager.NewVariableType() == "Float")
	{
		float value = 0.0f;
		CNodeTypeCollector::RegisterNewDataType(aName, static_cast<int>(CNodeDataManager::EDataType::EFloat));
		CNodeDataManager::Get()->SetData(aName, CNodeDataManager::EDataType::EFloat, value);
	}
	else if (aGraphManager.NewVariableType() == "Int")
	{
		int value = 0;
		CNodeTypeCollector::RegisterNewDataType(aName, static_cast<int>(CNodeDataManager::EDataType::EInt));
		CNodeDataManager::Get()->SetData(aName, CNodeDataManager::EDataType::EInt, value);
	}
	else if (aGraphManager.NewVariableType() == "Bool")
	{
		bool value = false;
		CNodeTypeCollector::RegisterNewDataType(aName, static_cast<int>(CNodeDataManager::EDataType::EBool));
		CNodeDataManager::Get()->SetData(aName, CNodeDataManager::EDataType::EBool, value);
	}
	else if (aGraphManager.NewVariableType() == "Start")
	{
		bool value = false;
		CNodeTypeCollector::RegisterNewDataType(aName, static_cast<int>(CNodeDataManager::EDataType::EStart));
		CNodeDataManager::Get()->SetData(aName, CNodeDataManager::EDataType::EStart, value);
	}
	else if (aGraphManager.NewVariableType() == "Vector 3")
	{
		Vector3 value = { 0.0f,0.0f,0.0f };
		CNodeTypeCollector::RegisterNewDataType(aName, static_cast<int>(CNodeDataManager::EDataType::EVector3));
		CNodeDataManager::Get()->SetData(aName, CNodeDataManager::EDataType::EVector3, value);
	}
	aGraphManager.AddCustomDataNode(aName);
}