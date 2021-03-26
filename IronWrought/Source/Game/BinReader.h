#pragma once
#include <fstream>

struct SVertexPaintColorData {
	std::vector<Vector3> myColors;
	std::vector<Vector3> myVertexPositions;
	int myVertexMeshID;
};

struct SVertexPaintCollection {
	std::vector<SVertexPaintColorData> myData;

	//std::map<int, SVertexPaintColorData> myVertexColorMap;
	//std::vector<SVertexPaintColorData> myCollection;
};

class CBinReader
{
public:
	CBinReader();
	~CBinReader();

	static SVertexPaintCollection LoadVertexPaintCollection(const std::string& aSceneName);
	static SVertexPaintColorData LoadVertexColorData(const std::string& aBinFilePath);

private:

	template<typename T>
	static size_t Read(T& aData, char* aStreamPtr, const unsigned int aCount = 1)
	{
		memcpy(&aData, aStreamPtr, sizeof(T) * aCount);
		return sizeof(T) * aCount;
	}

	static std::string ReadStringAuto(char* aStreamPtr)
	{
		int length = 0;
		aStreamPtr += Read(length, aStreamPtr);

		std::string text = "";
		aStreamPtr += ReadString(text, aStreamPtr, length);

		return text;
	}

	static size_t ReadString(std::string& data, char* aStreamPtr, size_t aLength)
	{
		data.reserve(aLength + 1);
		memcpy(&data.data()[0], aStreamPtr, sizeof(char) * aLength);
		data.data()[aLength] = '\0';
		return sizeof(char) * aLength;
	}
};