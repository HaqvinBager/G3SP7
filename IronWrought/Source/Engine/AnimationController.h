#include <string>
#include <map>
#include <vector>

// assimp include files. These three are usually needed.
#include "assimp/Importer.hpp"	//OO version Header!
#include "assimp/postprocess.h"
#include "assimp/LogStream.hpp"
#include <fstream>

#include "AnimMathFunc.h"
#include "Timer.h"

#define NUM_BONES_PER_VERTEX 4
#define TEMP_FRAMES_PER_SECOND 30.0f // Original was 25.0f

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
	aiMatrix4x4 myFinalTransformation;
};

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

	//Create Init Function?

	bool ImportRig(const std::string& anFBXFilePath = "");
	bool ImportAnimation(const std::string& fileName);
	bool InitFromScene(const aiScene* pScene);
	void LoadBones(uint aMeshIndex, const aiMesh* aMesh);

// Update functions
	void ReadNodeHeirarchy( const aiScene* aScene, float anAnimationTime, const aiNode* aNode
						   , const aiMatrix4x4& aParentTransform, int aStopAnimAtLevel);

	void ReadNodeHeirarchy( const aiScene* aFromScene, const aiScene* aToScene, float anAnimationTimeFrom
						   , float anAnimationTimeTo, const aiNode* aStartNodeFrom, const aiNode* aStartNodeTo
						   , const aiMatrix4x4& aParentTransform, int aStopAnimAtLevel);

	void SetBoneTransforms(std::vector<aiMatrix4x4>& aTransformsVector);
	void UpdateAnimationTimes();
// ! Update functions

	void BlendToAnimation(uint anAnimationIndex, bool anUpdateBoth = true, float aBlendDuration = 0.3f, bool aTemporary = false, float aTime = 0.0f);
	bool SetBlendTime(float aTime);
	uint GetMaxIndex();
	bool IsDoneBlending();

	void SetRotation(const aiVector3D& aRotation) { myRotation = aRotation; }
	aiVector3D GetRotation() { return myRotation; }

	void Animation0Index(int anIndex) { myAnim0Index = anIndex; }
	void Animation1Index(int anIndex) { myAnim1Index = anIndex; }
	const uint Animation0Index() { return myAnim0Index; }
	const uint Animation1Index() { return myAnim1Index; }
	const size_t GetNrOfAnimations() const { return myAnimations.size(); }

	const float AnimationDuration(uint anIndex);
	const float Animation0Duration();
	const float Animation1Duration();

	void ResetAnimationTimeCurrent() { myAnimationTime1 = 0.0f; }

private:
	bool AnimationIndexWithinRange(uint anIndex);
private:
	float myAnimationTime0;
	float myAnimationTime1;

	// Used to set the time it should take to blend from myAnimation1 to myAnimation0
	float myBlendingTime;
	float myBlendingTimeMul;
	float myPlayTime;

	// With a myBlendingTime of 0 myAnimIndex0 is played. Using lerp use: 0.0f, to play.
	uint myAnim0Index;
	// With a myBlendingTime of > 0 myAnimIndex1 is played. Using lerp use: 1.0f, to play.
	uint myAnim1Index;

	uint myNumOfBones;
	bool myUpdateBoth;
	bool myTemporary;

	aiVector3D myRotation;

	// Holds the animations that we play. Each animation modifies bonetransforms depending on animation time.
	std::vector<aiScene*>				myAnimations;
	aiMatrix4x4							myGlobalInverseTransform;
	std::map<std::string, uint>			myBoneMapping;
	std::vector<MeshEntry>				myEntries;
	std::vector<BoneInfoAnim>			myBoneInfo;
	std::vector<VertexBoneDataAnim>		myMass;

// No longer used 2021 02 01
	// Takes ownership if myAnimations. I.e Importer has ownership if aiScene.
	// Used for loading myAnimations. This is the FBX. Seems like an FBX can hold several animations?
	//std::vector<Assimp::Importer*>		myImporters;
};