#include <map>
#include "IronMath.h"
#include "AnimMathFunc.h"



class CAnimationController
{
private:  //Animation Structs
	struct Joint {
		aiMatrix4x4 myInverseBindPose;
		const char* myName;
	};

	struct JointPose
	{
		aiMatrix4x4 aNodeTransformation;
		aiQuaternion myRotKey;
		aiVector3D myPosKey;
		aiVector3D myScaleKey;
	};

public:
	CAnimationController();
	~CAnimationController();

	//Import
	bool ImportRig(const std::string& anFBXFilePath = "");// Todo: handle in factory
	bool ImportAnimation(const std::string& anFBXFilePath);

	//Animate
	void AnimateMotion(const int anIndex, const float aTick, std::array<aiMatrix4x4, 64>& outTransforms);
	void AnimateMotionBlend1D(const int someIndexes[2], const float someTicks[2], const float aBlend, std::array<aiMatrix4x4, 64>& outTransforms);
	void AnimateMotionBlend2D(const int someIndexes[3], const float someTicks[3], const float someBlends[3], std::array<aiMatrix4x4, 64>& outTransforms);
	void AnimateMotionBlendCorners(const int someIndexes[4], const float someTicks[4], const float someBlendValues[2], std::array<aiMatrix4x4, 64>& outTransforms);

	const int MotionCount() const;
	const float MotionDurationInTicks(int anIndex) const;
	const float MotionTPS(int anIndex) const;
	std::vector<std::string> GetMotionNames();
	const int AnimationIndex(const std::string& aMotionName);
	const std::vector<std::string>& GetMotionPaths() { return myMotionPaths; }

private:
	//Import
	bool InitFromScene(const aiScene* pScene);
	void LoadBones(uint aMeshIndex, const aiMesh* aMesh);
	//Animate
	void ReadNodeHeirarchy(const aiScene* aScene, float tick, const aiNode* aNode,/* std::vector<aiMatrix4x4>& outJointPoses,*/ std::vector<JointPose>& outPoses);
	void CalculateNodePoses(const aiScene* aScene, const aiNode* aNode, float aTick, std::vector<JointPose>& outJointPoses);
	void CalculateWorldTransforms(std::array<aiMatrix4x4, 64>& outTransforms, std::vector<JointPose>::const_iterator& aNodeTransformationIterator, const aiNode* aParentNode, const aiMatrix4x4& aParentTransform);
	aiMatrix4x4 BlendMatrix(const aiMatrix4x4& aMatrixA, const aiMatrix4x4& aMatrixB, float aBlend) const;
	unsigned int CountJoints(const aiNode* aNode);
	//Save
	void SaveMotionToMap(const aiScene* aScene, const std::string& aFilePath);
	// Holds the animations that we play. Each animation modifies bonetransforms depending on animation time.
	std::map<std::string, const aiScene*>	mySceneMap;
	std::vector<const aiScene*>				myScenes;// was std::vector<const aiScene*>
	std::vector<Joint>						myBoneInfo;
	aiMatrix4x4								myGlobalInverseTransform;
	std::map<std::string, uint>				myBoneMapping;
	std::vector<std::string>				myMotionPaths;

private:
	/// <summary>
	/// Interpolate for aiQuatKey, aiVectorKey 
	/// </summary>
	/// <param name="time">the current time * ticksPerSecond</param>
	template<class T>
	void Interpolate(T& out, const float time, const uint keyStart, const uint keyEnd, const T* someKeys)
	{
		//const float percent = InvLerp(someKeys[keyStart].mTime, someKeys[keyEnd].mTime, time);
		const float percent = IronMath::Remap(someKeys[keyStart].mTime, someKeys[keyEnd].mTime, 0.0f, 1.0f, time);
		Assimp::Interpolator<T>()(out, someKeys[keyStart], someKeys[keyEnd], time);
	}


};

#pragma region Old and Beautiful
//struct SkeletonPose {
//	~SkeletonPose() { mySkeleton = nullptr; }
//
//	Skeleton* mySkeleton;
//	std::vector<JointPose> myLocalJointPoses;
//	std::vector<aiMatrix4x4> myGlobalTransformations;
//};

//struct AnimationSample {
//	std::vector<JointPose> myJointPoses;
//};
//
//struct AnimationClip {
//	Skeleton* mySkeleton;
//	float myFPS;
//	float myTickCount;
//	std::vector<AnimationSample> mySamples;
//	bool myIsLooping;
//};
//Attempting Hype END

//Skeleton mySkeleton;
//std::unique_ptr<SkeletonPose> mySkeletonPose;
/*std::vector<float> myTicks;
float myTicks0;
float myTicks1;*/
//// Used to set the time it should take to blend from myAnimation1 to myAnimation0
//float myBlendingTime;
//float myBlendingTimeMul;
//float myPlayTime;
//// With a myBlendingTime of 0 myAnimIndex0 is played. Using lerp use: 0.0f, to play.
//uint myAnimIndex0;
//// With a myBlendingTime of > 0 myAnimIndex1 is played. Using lerp use: 1.0f, to play.
//uint myAnimIndex1;
//uint myNumOfBones;
//bool myUpdateBoth;
//bool myTemporary;
//aiVector3D myRotation;

	//struct SerializedObject {
	//	SerializedObject(CAnimationController& data)
	//	{
	//		myAnimationTime0 = &data.myTicks0;
	//		myAnimationTime1 = &data.myTicks1;
	//		myBlendingTime = &data.myBlendingTime;
	//		myBlendingTimeMul = &data.myBlendingTimeMul;;
	//		myPlayTime = &data.myPlayTime;
	//		myAnim0Index = &data.myAnimIndex0;
	//		myAnim1Index = &data.myAnimIndex1;
	//		myNumOfBones = &data.myNumOfBones;
	//		myUpdateBoth = &data.myUpdateBoth;
	//		myTemporary = &data.myTemporary;
	//		myRotation = data.myRotation;
	//		myAnimations = data.myScenes;
	//		myGlobalInverseTransform = data.myGlobalInverseTransform;
	//		myBoneMapping = data.myBoneMapping;
	//		myEntries = data.myEntries;
	//		myBoneInfo = data.myBoneInfo;
	//		myMass = data.myMass;
	//	}

	//	float* myAnimationTime0;
	//	float* myAnimationTime1;
	//	float* myBlendingTime;
	//	float* myBlendingTimeMul;
	//	float* myPlayTime;
	//	uint* myAnim0Index;
	//	uint* myAnim1Index;
	//	uint* myNumOfBones;
	//	bool* myUpdateBoth;
	//	bool* myTemporary;
	//	aiVector3D							myRotation;
	//	std::vector<const aiScene*>			myAnimations;
	//	aiMatrix4x4							myGlobalInverseTransform;
	//	std::map<std::string, uint>			myBoneMapping;
	//	std::vector<MeshEntry>				myEntries;
	//	std::vector<Joint>			myBoneInfo;
	//	std::vector<VertexBoneDataAnim>		myMass;
	//};

//void ImportSkeleton(const std::string& aSkeletonFBXPath);
	//void ReadCreateSkeleton(const aiNode* aNode, Skeleton& outSkeleton);
	//bool AnimationIndexWithinRange(int anIndex) const;
	//unsigned int CountChildren(const aiNode* aNode);

	//std::vector<MeshEntry>				myEntries;
	//std::map<const char*, BoneInfoAnim>		myBoneInfoMap;
	//std::vector<VertexBoneDataAnim>		myMass;
	//std::vector<CBlendTree>					myBlendTrees;

//#include "Animator.h"
//#include "BlendTree.h"

//#define ANIMATION_DURATION_IN_MILLISECONDS// AS of 2021 02 23 is not used

//#define NUM_BONES_PER_VERTEX 4
//#define TEMP_FRAMES_PER_SECOND 24.0f//30.0f // Original was 25.0f
//
//#define ANIMATED_AT_FRAMES_PER_SECOND 30.0f//30.0f // Original was 25.0f
//
//#ifdef _DEBUG
//#define ANIMATION_SAFE_MODE
//#endif
//struct VertexBoneDataAnim
//{
//	uint myIDs[NUM_BONES_PER_VERTEX];
//	float myWeights[NUM_BONES_PER_VERTEX];
//
//	VertexBoneDataAnim()
//	{
//		Reset();
//	};
//
//	void Reset()
//	{
//		memset(myIDs, 0, sizeof(myIDs));
//		memset(myWeights, 0, sizeof(myWeights));
//	}
//
//	void AddBoneData(uint BoneID, float Weight)
//	{
//		for (uint i = 0; i < NUM_BONES_PER_VERTEX; i++)
//		{
//			if (myWeights[i] == 0.0f)
//			{
//				myIDs[i] = BoneID;
//				myWeights[i] = Weight;
//				return;
//			}
//		}
//		// Should never get here - more bones than we have space for
//		assert(false);
//	}
//};

//struct BoneInfoAnim
//{
//	aiMatrix4x4 myBoneOffset;
//};

//struct Skeleton {
//	std::vector<Joint> myJoints;
//};


//#define INVALID_MATERIAL 0xFFFFFFFF;

//struct MeshEntry
//{
//	MeshEntry()
//	{
//		myNumIndices = 0;
//		myBaseVertex = 0;
//		myBaseIndex = 0;
//		myMaterialIndex = INVALID_MATERIAL;
//	}
//
//	unsigned int myNumIndices;
//	unsigned int myBaseVertex;
//	unsigned int myBaseIndex;
//	unsigned int myMaterialIndex;
//};

//#include "ModelMath.h"

#pragma endregion
