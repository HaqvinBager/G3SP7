#include <string>
#include <map>
#include <vector>

// assimp include files. These three are usually needed.
#include "assimp/Importer.hpp"	//OO version Header!
#include "assimp/postprocess.h"
#include "assimp/LogStream.hpp"
#include <fstream>

#include "AnimMathFunc.h"
#include "ModelMath.h"
#include "IronMath.h"

//#include "Animator.h"
#include "BlendTree.h"

//#define ANIMATION_DURATION_IN_MILLISECONDS// AS of 2021 02 23 is not used

#define NUM_BONES_PER_VERTEX 4
#define TEMP_FRAMES_PER_SECOND 24.0f//30.0f // Original was 25.0f

#define ANIMATED_AT_FRAMES_PER_SECOND 30.0f//30.0f // Original was 25.0f

#ifdef _DEBUG
#define ANIMATION_SAFE_MODE
#endif
struct VertexBoneDataAnim
{
	uint myIDs[NUM_BONES_PER_VERTEX];
	float myWeights[NUM_BONES_PER_VERTEX];

	VertexBoneDataAnim()
	{
		Reset();
	};

	void Reset()
	{
		memset(myIDs, 0, sizeof(myIDs));
		memset(myWeights, 0, sizeof(myWeights));
	}

	void AddBoneData(uint BoneID, float Weight)
	{
		for (uint i = 0; i < NUM_BONES_PER_VERTEX; i++)
		{
			if (myWeights[i] == 0.0f)
			{
				myIDs[i] = BoneID;
				myWeights[i] = Weight;
				return;
			}
		}
		// Should never get here - more bones than we have space for
		assert(false);
	}
};

struct BoneInfoAnim
{
	aiMatrix4x4 myBoneOffset;
};

//Attempting Hype
struct Joint {
	/*Joint(aiMatrix4x4& aTransformation, const char* aName, int aParentIndex)
	{
		myInverseBindPose = aTransformation;
		myName = aName;
		myParentIndex = static_cast<INT8>(aParentIndex);
	}*/

	aiMatrix4x4 myInverseBindPose;
	const char* myName;
	//INT8 myParentIndex;
};

struct Skeleton {
	//UINT8 jointCount;
	std::vector<Joint> myJoints;
};

struct JointPose
{
	aiMatrix4x4 aNodeTransformation;
	aiQuaternion myRotKey;
	aiVector3D myPosKey;
	aiVector3D myScaleKey;
};

struct SkeletonPose {
	~SkeletonPose() { mySkeleton = nullptr; }

	Skeleton* mySkeleton;
	std::vector<JointPose> myLocalJointPoses;
	std::vector<aiMatrix4x4> myGlobalTransformations;
};

struct AnimationSample {
	std::vector<JointPose> myJointPoses;
};

struct AnimationClip {
	Skeleton* mySkeleton;
	float myFPS;
	float myTickCount;
	std::vector<AnimationSample> mySamples;
	bool myIsLooping;
};
//Attempting Hype END

#define INVALID_MATERIAL 0xFFFFFFFF;

struct MeshEntry
{
	MeshEntry()
	{
		myNumIndices = 0;
		myBaseVertex = 0;
		myBaseIndex = 0;
		myMaterialIndex = INVALID_MATERIAL;
	}

	unsigned int myNumIndices;
	unsigned int myBaseVertex;
	unsigned int myBaseIndex;
	unsigned int myMaterialIndex;
};


class CAnimationController
{
public:
	CAnimationController();
	~CAnimationController();

	//Import
	bool ImportRig(const std::string& anFBXFilePath = "");// Todo: handle in factory
	bool ImportAnimation(const std::string& anFBXFilePath);
	void ImportSkeleton(const std::string& aSkeletonFBXPath);

	//Animate
	void SetBoneTransforms(int animIndex0, float aTick, std::array<aiMatrix4x4, 64>& outTransforms);
	void SetBoneTransforms(int animIndex0, int animIndex1, float aTick0, float aTick1, float blend, std::array<aiMatrix4x4, 64>& outTransforms);
	void SetBoneTransforms(int indexes[3], Vector3 someTicks, Vector3 someBlends, std::array<aiMatrix4x4, 64>& outTransforms);


	void BlendSpace2D(const Vector4& indexes, const Vector2& blendValues, const Vector4& ticks, std::array<aiMatrix4x4, 64>& outTransforms);


	const int AnimationCount() const;
	const float AnimationDurationInTicks(int anIndex) const;
	const float AnimationTPS(int anIndex) const;
	std::vector<std::string> GetMotionNames();
	const int AnimationIndex(const std::string& aMotionName);


private:
	//Import
	bool InitFromScene(const aiScene* pScene);
	void LoadBones(uint aMeshIndex, const aiMesh* aMesh);

	//Animate
	void ReadNodeHeirarchy(const aiScene* aScene, float tick, const aiNode* aNode,/* std::vector<aiMatrix4x4>& outJointPoses,*/ std::vector<JointPose>& outPoses);
	void CalculateNodePoses(const aiScene* aScene, const aiNode* aNode, float aTick, std::vector<JointPose>& outJointPoses);
	void CalculateWorldTransforms(std::array<aiMatrix4x4, 64>& outTransforms, std::vector<JointPose>::const_iterator& aNodeTransformationIterator, const aiNode* aParentNode, const aiMatrix4x4& aParentTransform);


private:
	//bool AnimationIndexWithinRange(int anIndex) const;
	void SaveMotionToMap(const aiScene* aScene, const std::string& aFilePath);

	aiMatrix4x4 BlendMatrix(const aiMatrix4x4& aMatrixA, const aiMatrix4x4& aMatrixB, float aBlend) const;
	unsigned int CountJoints(const aiNode* aNode);

	void ReadCreateSkeleton(const aiNode* aNode, Skeleton& outSkeleton);
	//unsigned int CountChildren(const aiNode* aNode);

private:

	Skeleton mySkeleton;
	std::unique_ptr<SkeletonPose> mySkeletonPose;


	std::vector<float> myTicks;

	float myTicks0;
	float myTicks1;

	// Used to set the time it should take to blend from myAnimation1 to myAnimation0
	float myBlendingTime;
	float myBlendingTimeMul;
	float myPlayTime;

	// With a myBlendingTime of 0 myAnimIndex0 is played. Using lerp use: 0.0f, to play.
	uint myAnimIndex0;
	// With a myBlendingTime of > 0 myAnimIndex1 is played. Using lerp use: 1.0f, to play.
	uint myAnimIndex1;

	uint myNumOfBones;
	bool myUpdateBoth;
	bool myTemporary;

	aiVector3D myRotation;

	// Holds the animations that we play. Each animation modifies bonetransforms depending on animation time.
	std::map<std::string, const aiScene*> mySceneMap;
	std::vector<const aiScene*>			myScenes;// was std::vector<const aiScene*>
	std::vector<Joint>			myBoneInfo;


	aiMatrix4x4							myGlobalInverseTransform;
	std::map<std::string, uint>			myBoneMapping;
	std::vector<MeshEntry>				myEntries;
	//std::map<const char*, BoneInfoAnim>		myBoneInfoMap;
	std::vector<VertexBoneDataAnim>		myMass;
	std::vector<std::string>			myMotionPaths;
	std::vector<CBlendTree>				myBlendTrees;


public:

	uint GetCurrentAnimationIndex0()
	{
		return myAnimIndex0;
	}

	std::vector<std::string>& GetMotionPaths()
	{
		return myMotionPaths;
	}

	struct SerializedObject {
		SerializedObject(CAnimationController& data)
		{
			myAnimationTime0 = &data.myTicks0;
			myAnimationTime1 = &data.myTicks1;
			myBlendingTime = &data.myBlendingTime;
			myBlendingTimeMul = &data.myBlendingTimeMul;;
			myPlayTime = &data.myPlayTime;
			myAnim0Index = &data.myAnimIndex0;
			myAnim1Index = &data.myAnimIndex1;
			myNumOfBones = &data.myNumOfBones;
			myUpdateBoth = &data.myUpdateBoth;
			myTemporary = &data.myTemporary;
			myRotation = data.myRotation;
			myAnimations = data.myScenes;
			myGlobalInverseTransform = data.myGlobalInverseTransform;
			myBoneMapping = data.myBoneMapping;
			myEntries = data.myEntries;
			myBoneInfo = data.myBoneInfo;
			myMass = data.myMass;
		}

		float* myAnimationTime0;
		float* myAnimationTime1;
		float* myBlendingTime;
		float* myBlendingTimeMul;
		float* myPlayTime;
		uint* myAnim0Index;
		uint* myAnim1Index;
		uint* myNumOfBones;
		bool* myUpdateBoth;
		bool* myTemporary;
		aiVector3D							myRotation;
		std::vector<const aiScene*>			myAnimations;
		aiMatrix4x4							myGlobalInverseTransform;
		std::map<std::string, uint>			myBoneMapping;
		std::vector<MeshEntry>				myEntries;
		std::vector<Joint>			myBoneInfo;
		std::vector<VertexBoneDataAnim>		myMass;
	};

private:
	/// <summary>
	/// Interpolate for aiQuatKey, aiVectorKey 
	/// </summary>
	/// <param name="time">the current time * ticksPerSecond</param>
	template<class T>
	void Interpolate(T& out, const float time, const uint keyStart, const uint keyEnd, const T* someKeys)
	{
		//const float percent = InvLerp(someKeys[keyStart].mTime, someKeys[keyEnd].mTime, time);
		const float percent = Remap(someKeys[keyStart].mTime, someKeys[keyEnd].mTime, 0.0f, 1.0f, time);
		Assimp::Interpolator<T>()(out, someKeys[keyStart], someKeys[keyEnd], time);
	}


	float Lerp(float a, float b, float t)
	{
		return (1.0f - t) * a + b * t;
	}

	float InvLerp(float a, float b, float v)
	{
		return (v - a) / (b - a);
	}

	float Remap(float inMin, float inMax, float outMin, float outMax, float v)
	{
		float t = InvLerp(inMin, inMax, v);
		return Lerp(outMin, outMax, t);
	}
};

//void UpdateAnimationTimes(std::array<SlimMatrix44, 64>& someBones);

	//const int Animation0Index() const { return static_cast<int>(myAnimIndex0); }
	//const int Animation1Index() const { return static_cast<int>(myAnimIndex1); }

	//const float AnimationTPS(const std::string& aMotionName);
	//
	//const float AnimationDurationInSeconds(int anIndex) const;

	//void AnimationIndex0(int anIndex) { myAnimIndex0 = static_cast<uint>(anIndex); }
	//void AnimationIndex1(int anIndex) { myAnimIndex1 = static_cast<uint>(anIndex); }
	//void SetAnimationIndexTick0(const float aTick) { myTicks0 = aTick; }
	//void SetAnimationIndexTick1(const float aTick) { myTicks1 = aTick; }
	//void SetBlendingTime(const float aBlendValue) { myBlendingTime = aBlendValue; }

	//CBlendTree* AddBlendTree(const char* aName);
	//std::vector<CBlendTree>& GetBlendTrees() { return myBlendTrees; }