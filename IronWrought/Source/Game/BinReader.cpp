#include "stdafx.h"
#include "BinReader.h"
#include "EngineDefines.h"
#include <EngineException.h>
#include "FolderUtility.h"

CBinReader::CBinReader()
{
}

CBinReader::~CBinReader()
{
}

SVertexPaintCollection CBinReader::LoadVertexPaintCollection(const std::string& aSceneName)
{
	std::string exportPath = ASSETPATH("Assets/Generated/" + aSceneName + "/VertexColors/");
	std::vector<std::string> binPaths = CFolderUtility::GetFileNamesInFolder(exportPath, ".bin");

	if(binPaths.size() == 0)
		return SVertexPaintCollection{ };

	std::ifstream stream;
	stream.open(ASSETPATH("Assets/Generated/" + aSceneName + "/VertexColors/" + binPaths[0]), std::ios::binary);
	if (!stream.is_open())
	{
		ENGINE_BOOL_POPUP(stream.is_open(), "Failed to open Binary File: %s", /*aBinFilePath.c_str()*/ "");
		return SVertexPaintCollection{ };
	}

	std::string binaryData((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());
	char* ptr = &binaryData[0];

	int count = 0;
	ptr += Read(count, ptr);
	SVertexPaintCollection vertexPaintCollection = {};
	vertexPaintCollection.myData.reserve(count);
	for (int i = 0; i < count; ++i)
	{
		SVertexPaintColorData data = { };
		ptr += Read(data.myVertexMeshID, ptr);
		int colorCount = 0;

		ptr += Read(colorCount, ptr);
		data.myColors.resize(colorCount);
		ptr += Read(data.myColors.data()[0], ptr, colorCount);

		int vertexCount = 0;
		ptr += Read(vertexCount, ptr);
		data.myVertexPositions.resize(vertexCount);
		ptr += Read(data.myVertexPositions.data()[0], ptr, vertexCount);

		vertexPaintCollection.myData.push_back(data);
	}

	return vertexPaintCollection;






	//SVertexPaintCollection collection = {};
	//collection.myCollection.reserve(binPaths.size());
	//for (auto binPath : binPaths)
	//{
	//	collection.myData.push_back(LoadVertexColorData(0));
	//}
	//return std::move(collection);

	//Nästa steg är att ladda in datan med det nya formatet <3
}

SVertexPaintColorData CBinReader::LoadVertexColorData(const std::string& aBinFilePath)
{

	std::ifstream stream;
	stream.open(aBinFilePath, std::ios::binary);
	if (!stream.is_open())
	{
		ENGINE_BOOL_POPUP(stream.is_open(), "Failed to open Binary File: %s", /*aBinFilePath.c_str()*/ "");
		return SVertexPaintColorData{ };
	}

	std::string binaryData((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());
	char* ptr = &binaryData[0];

	SVertexPaintColorData data = { };
	ptr += Read(data.myVertexMeshID, ptr);
	int colorCount = 0;
	ptr += Read(colorCount, ptr);
	data.myColors.resize(colorCount);
	ptr += Read(data.myColors.data()[0], ptr, colorCount);

	int vertexCount = 0;
	ptr += Read(vertexCount, ptr);
	data.myVertexPositions.resize(vertexCount);
	ptr += Read(data.myVertexPositions.data()[0], ptr, vertexCount);

	return std::move(data);
}

void CBinReader::Test(const std::string& aBinFile)
{
	std::ifstream stream;
	stream.open(aBinFile, std::ios::binary);
	if (!stream.is_open())
		ENGINE_BOOL_POPUP(stream.is_open(), "Failed to open Binary File: %s", /*aBinFilePath.c_str()*/ "");
	
	std::string binaryData((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());

	char* ptr = &binaryData[0];
	int idCount = 0;
	ptr += Read(idCount, ptr);

	std::vector<int> ids;
	ids.resize(idCount);
	ptr += Read(ids.data()[0], ptr, idCount);

	int transformCount = 0;
	ptr += Read(transformCount, ptr);

	std::vector<STransformTest> transformData;
	transformData.resize(transformCount);
	ptr += Read(transformData.data()[0], ptr, transformCount);

	STransformTest& testdata = transformData[0];
	testdata;


}

/*
	struct SLevelData {
		std::vector<SInstanceID> myInstanceIDs;
		std::vector<STransform> myTransforms;
		std::vector<SModel> myModels;
		SDirectionalLightData myDirectionalLight;
		std::vector<SPointLight> myPointLights;
		SPlayer myPlayer;
		std::vector<SCollider> myColliders;
		std::vector<SEnemy> myEnemies;
		std::vector<SParentData> myParents;
		std::vector<SEventData> myEvents;
		std::vector<SInstancedModel> myInstancedModels;
	};
*/

Binary::SLevelData CBinReader::Load(const std::string& aPath)
{
	std::ifstream stream;
	stream.open(aPath, std::ios::binary);
	if (!stream.is_open())
		ENGINE_BOOL_POPUP(stream.is_open(), "Failed to open Binary File");

	std::string binaryData((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());
	char* ptr = &binaryData[0];

	Binary::SLevelData data = {};
	int count = 0;

	ptr += Read(count, ptr);						//Read Instance IDs
	data.myInstanceIDs.resize(count);
	ptr += Read(data.myInstanceIDs.data()[0], ptr, count);

	ptr += Read(count, ptr);						//Read Transforms
	data.myTransforms.resize(count);
	ptr += Read(data.myTransforms.data()[0], ptr, count);

	ptr += Read(count, ptr);						//Read Model Links
	data.myModels.resize(count);
	ptr += Read(data.myModels.data()[0], ptr, count); 

	ptr += Read(data.myDirectionalLight, ptr);		//Read Directional Light

	ptr += Read(count, ptr);
	data.myPointLights.resize(count);
	ptr += Read(data.myPointLights.data()[0], ptr, count);

	ptr += Read(data.myPlayer.instanceID, ptr);		//Read Player Instance ID
	ptr += Read(count, ptr);						//Read Player Children IDs
	data.myPlayer.childrenIDs.resize(count);
	ptr += Read(data.myPlayer.childrenIDs.data()[0], ptr, count);

	ptr += Read(count, ptr);						//Read Colliders
	data.myColliders.resize(count);
	ptr += Read(data.myColliders.data()[0], ptr, count);
	
	//TODO Unity Export temporarily removed for Enemies until the export is Binary Friendly <3 /Axel Savage 2021-04-23
	/*ptr += Read(count, ptr);
	data.myEnemies.resize(count);
	ptr += Read(data.myEnemies.data()[0], ptr, count);*/

	ptr += Read(count, ptr);						//Read Parents
	data.myParents.resize(count);
	for (int i = 0; i < count; ++i)
	{
		ptr += Read(data.myParents[i].parent, ptr);	
		int childCount = 0;
		ptr += Read(childCount, ptr);
		data.myParents[i].children.resize(childCount);
		ptr += Read(data.myParents[i].children.data()[0], ptr, childCount);
	}

	ptr += Read(count, ptr);						//Read Events
	data.myEvents.resize(count);
	for (int i = 0; i < count; ++i)
	{
		ptr += Read(data.myEvents[i].instanceID, ptr);
		ptr += ReadCharBuffer(ptr, data.myEvents[i].gameEvent);
	}

	ptr += Read(count, ptr);						//Read Instanced Models
	data.myInstancedModels.resize(count);
	for (int i = 0; i < count; ++i)
	{
		ptr += Read(data.myInstancedModels[i].assetID, ptr);
		int instanceCount = 0;
		ptr += Read(instanceCount, ptr);
		data.myInstancedModels[i].transforms.resize(instanceCount);
		ptr += Read(data.myInstancedModels[i].transforms.data()[0], ptr, instanceCount);
	}

	stream.close();
	ptr = nullptr;
	binaryData.clear();
	
	stream.open(ASSETPATH("Assets/Generated/Resources_Bin/Resources.bin"), std::ios::binary);
	if (!stream.is_open())
		ENGINE_BOOL_POPUP(stream.is_open(), "Failed to open Binary File");

	std::string binaryDataTest((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());
	ptr = &binaryDataTest[0];

	int resourceCount = 0;
	ptr += Read(resourceCount, ptr);

	Binary::SResources resources = {};
	resources.myPaths.reserve(resourceCount);
	for (int i = 0; i < resourceCount; ++i)
	{
		std::string resourceString = {};
		ptr += ReadCharBuffer(ptr, resourceString);
		resources.myPaths.push_back(resourceString);
	}

	return std::move(data);
}
