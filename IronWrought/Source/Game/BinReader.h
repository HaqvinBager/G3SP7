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

struct STransformTest {
	Vector3 pos;
	Vector3 rot;
	Vector3 scale;
};

namespace Binary {
	struct SInstanceID {
		int instanceID;
	};
	struct STransform {
		int instanceID;
		Vector3 pos;
		Vector3 rot;
		Vector3 scale;
	};

	struct SInstancedTransform {
		Vector3 pos;
		Vector3 rot;
		Vector3 scale;
	};
	struct SModel {
		int instanceID;
		int assetID;
		int vertexColorID;
	};
	struct SDirectionalLightData
	{
		int instanceID;
		Vector3 direction;
		float r;
		float g;
		float b;
		float intensity;
		int assetID;
		//std::string cubemapName;
	};
	struct SPointLight
	{
		int instanceID;
		float range;
		Vector3 color;
		float intensity;
	};
	struct SPlayer
	{
		int instanceID;
		std::vector<int> childrenIDs;
	};
	struct SCollider
	{
		int instanceID;
		Vector3 positionOffest;
		int colliderType;
		Vector3 boxSize;
		float sphereRadius;
		float capsuleHeight;
		float capsuleRadius;
		int isStatic;
		int isKinematic;
		int isTrigger;
		float mass;
		Vector3 localMassPosition;
		Vector3 inertiaTensor;
		float dynamicFriction;
		float staticFriction;
		float bounciness;
	};
	struct SEnemy
	{
		int instanceID;
		float speed;
		float radius;
		float health;
		float attackDistance;
	};
	struct SParentData
	{
		int parent;
		std::vector<int> children;
	};
	struct SEventData
	{
		int instanceID;
		std::string gameEvent;
	};
	struct SInstancedModel
	{
		int assetID;
		std::vector<SInstancedTransform> transforms;
	};

	struct SLevelData {
		std::vector<SInstanceID> myInstanceIDs;
		std::vector<STransform> myTransforms;
		std::vector<SModel> myModels;
		SDirectionalLightData myDirectionalLight;
		std::vector<SPointLight> myPointLights;
		SPlayer myPlayer;
		std::vector<SCollider> myColliders;
		//std::vector<SEnemy> myEnemies;
		std::vector<SParentData> myParents;
		std::vector<SEventData> myEvents;
		std::vector<SInstancedModel> myInstancedModels;
	};

	struct SResources {
		std::vector<std::string> myPaths;
	};
}

class CBinReader
{
	friend class CSceneFactory;
public:
	CBinReader();
	~CBinReader();

	static SVertexPaintCollection LoadVertexPaintCollection(const std::string& aSceneName);
	static SVertexPaintColorData LoadVertexColorData(const std::string& aBinFilePath);
	static void Test(const std::string& aBinFile);

	Binary::SLevelData Load(const std::string& aPath);


private:
	


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

	size_t ReadCharBuffer(char* aPtr, std::string& outString)
	{
		char buffer[128];// = new char[aPtr[0]];
		memcpy(&buffer[0], aPtr + 1, aPtr[0]);
		buffer[aPtr[0]] = '\0';
		outString.resize(aPtr[0]);
		memcpy(&outString.data()[0], &aPtr[1], aPtr[0]);
		return sizeof(char) * aPtr[0] + 1;
	}

	static size_t ReadString(std::string& data, char* aStreamPtr, size_t aLength)
	{
		data.reserve(aLength + 1);
		memcpy(&data.data()[0], aStreamPtr, sizeof(char) * aLength);
		data.data()[aLength] = '\0';
		return sizeof(char) * aLength;
	}
};