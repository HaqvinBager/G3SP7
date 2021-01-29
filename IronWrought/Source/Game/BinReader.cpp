#include "stdafx.h"
#include "BinReader.h"
#include <EngineException.h>

CBinReader::CBinReader()
{
}

CBinReader::~CBinReader()
{

}

SVertexPaintColorData CBinReader::LoadVertexColorData(const std::string& aBinFilePath)
{
	std::ifstream stream;
	stream.open(aBinFilePath, std::ios::binary);
	ENGINE_BOOL_POPUP(stream.is_open(), "Failed to open Binary File: %s", aBinFilePath.c_str());

	std::string binaryData((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());
	char* ptr = &binaryData[0];

	SVertexPaintColorData data = { };
	ptr += Read(data.myVertexMeshID, ptr);
	int colorCount = 0;
	ptr += Read(colorCount, ptr);
	data.myColors.resize(colorCount);
	ptr += Read(data.myColors.data()[0], ptr, colorCount);

	return std::move(data);
}
