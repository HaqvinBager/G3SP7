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
