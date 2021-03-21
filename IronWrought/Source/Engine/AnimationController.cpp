#include "stdafx.h"
#include "AnimationController.h"
#include "Timer.h"
#include <AssImp\anim.h>
#include <math.h>


//constexpr float f_milliseconds = 1000.0f;
#ifdef _DEBUG
//#define ALLOW_PRINT
#endif
#include <iomanip>
void TEMP_PrintMatrix4x4_2decimals(const aiMatrix4x4& aMatrix, const std::string& aMsg = "")
{
	aMatrix; aMsg;
#ifdef ALLOW_PRINT
	// https://stackoverflow.com/questions/5907031/printing-the-correct-number-of-decimal-points-with-cout
	std::cout << std::fixed;
	std::cout << std::setprecision(2);

	std::cout << aMsg << std::endl;
	std::cout << aMatrix.a1 << " | " << aMatrix.a2 << " | " << aMatrix.a3 << " | " << aMatrix.a4 << std::endl;
	std::cout << aMatrix.b1 << " | " << aMatrix.b2 << " | " << aMatrix.b3 << " | " << aMatrix.b4 << std::endl;
	std::cout << aMatrix.c1 << " | " << aMatrix.c2 << " | " << aMatrix.c3 << " | " << aMatrix.c4 << std::endl;
	std::cout << aMatrix.d1 << " | " << aMatrix.d2 << " | " << aMatrix.d3 << " | " << aMatrix.d4 << std::endl;
	std::cout << "------" << std::endl;
#endif
}

CAnimationController::CAnimationController()
	: myNumOfBones(0)
	, myAnimIndex0(0)
	, myAnimIndex1(0)
	, myRotation(0.f)
	, myBlendingTime(0.f)
	, myBlendingTimeMul(1.f)
	, myUpdateBoth(true)
	, myTemporary(false)
	, myPlayTime(0.f)
	, myTicks1(0)
	, myTicks0(0)
{

}
CAnimationController::~CAnimationController()
{
	for (size_t i = 0; i < myScenes.size(); ++i)
	{
		delete myScenes[i];
		myScenes[i] = nullptr;
	}
	myScenes.clear();
}

bool CAnimationController::ImportRig(const std::string& anFBXFilePath/*, const std::vector<std::string>& someAnimationPaths*/)
{
	if (anFBXFilePath.length() <= 0)
		return false;

	// Check if file exists
	std::ifstream fIn(anFBXFilePath.c_str());
	if (!fIn.fail())
	{
		fIn.close();
	}
	else
	{
		MessageBoxA(NULL, ("Couldn't open file: " + anFBXFilePath).c_str(), "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return false;
	}

	myAnimIndex0 = static_cast<int>(myScenes.size());

	Assimp::Importer importer;
	const aiScene* scene = nullptr;
	if (importer.ReadFile(anFBXFilePath, aiProcessPreset_TargetRealtime_Quality | aiProcess_ConvertToLeftHanded))
	{
		scene = myScenes.emplace_back(importer.GetOrphanedScene());
		SaveMotionToMap(myScenes.back(), anFBXFilePath);
	}
	else
	{
		return false;
	}

	myGlobalInverseTransform = myScenes[myAnimIndex0]->mRootNode->mTransformation;
	myGlobalInverseTransform.Inverse();

	// We're done. Everything will be cleaned up by the importer destructor
	return InitFromScene(myScenes[myAnimIndex0]);;
}
bool CAnimationController::ImportAnimation(const std::string& anFBXFilePath)
{
	// Check if file exists
	std::ifstream fin(anFBXFilePath.c_str());
	if (!fin.fail())
		fin.close();
	else
	{
		MessageBoxA(NULL, ("Couldn't open file: " + anFBXFilePath).c_str(), "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return false;
	}

	Assimp::Importer importer;
	myAnimIndex0 = static_cast<int>(myScenes.size());
	if (importer.ReadFile(anFBXFilePath, aiProcessPreset_TargetRealtime_Quality | aiProcess_ConvertToLeftHanded))
	{
		myScenes.push_back(importer.GetOrphanedScene());
		SaveMotionToMap(myScenes.back(), anFBXFilePath);
	}
	else
		return false;

	myMotionPaths.push_back(anFBXFilePath);

	// If the import failed, report it
	if (!myScenes[myAnimIndex0])
	{
		//logInfo(importer.GetErrorString());
		return false;
	}

	if (myScenes[myAnimIndex0]->mNumAnimations <= 0)
	{
		ENGINE_ERROR_BOOL_MESSAGE(false, std::string("Animation could not be loaded: " + anFBXFilePath).c_str());
		return false;
	}

	return true;
}
bool CAnimationController::InitFromScene(const aiScene* pScene)
{
	myTicks0 = 0.0f;
	myTicks1 = 0.0f;

	myEntries.resize(pScene->mNumMeshes);

	uint NumVertices = 0;
	uint NumIndices = 0;

	// Count the number of vertices and indices
	for (uint i = 0; i < myEntries.size(); i++)
	{
		myEntries[i].myMaterialIndex = pScene->mMeshes[i]->mMaterialIndex;
		myEntries[i].myNumIndices = pScene->mMeshes[i]->mNumFaces * 3;
		myEntries[i].myBaseVertex = NumVertices;
		myEntries[i].myBaseIndex = NumIndices;

		NumVertices += pScene->mMeshes[i]->mNumVertices;
		NumIndices += myEntries[i].myNumIndices;
	}

	myMass.resize(NumVertices);

	for (uint i = 0; i < pScene->mNumMeshes; ++i)
	{
		LoadBones(i, pScene->mMeshes[i]);
	}
	return true;
}
void CAnimationController::LoadBones(uint aMeshIndex, const aiMesh* aMesh)
{
	for (uint i = 0; i < aMesh->mNumBones; i++)
	{
		uint BoneIndex = 0;
		std::string BoneName(aMesh->mBones[i]->mName.data);

		if (myBoneMapping.find(BoneName) == myBoneMapping.end())
		{
			BoneIndex = myNumOfBones;
			myNumOfBones++;
			BoneInfoAnim bi;
			myBoneInfo.push_back(bi);
		}
		else
		{
			BoneIndex = myBoneMapping[BoneName];
		}

		myBoneMapping[BoneName] = BoneIndex;
		myBoneInfo[BoneIndex].myBoneOffset = aMesh->mBones[i]->mOffsetMatrix;

		for (uint j = 0; j < aMesh->mBones[i]->mNumWeights; j++)// Mass for physics?
		{
			uint VertexID = myEntries[aMeshIndex].myBaseVertex + aMesh->mBones[i]->mWeights[j].mVertexId;
			float Weight = aMesh->mBones[i]->mWeights[j].mWeight;
			myMass[VertexID].AddBoneData(BoneIndex, Weight);
		}
	}
}

void CAnimationController::SetBoneTransforms(std::array<aiMatrix4x4, 64>& aTransformsVector)
{
	aiMatrix4x4 Identity;
	InitIdentityM4(Identity);

	if (myAnimIndex0 == myAnimIndex1)
	{
		ReadNodeHeirarchy(
			myScenes[myAnimIndex0]
			, myTicks0
			, myScenes[myAnimIndex0]->mRootNode
			, Identity);
	}
	else
	{
		ReadNodeHeirarchy(
			myScenes[myAnimIndex0]
			, myScenes[myAnimIndex1]
			, myTicks0
			, myTicks1
			, myScenes[myAnimIndex0]->mRootNode
			, myScenes[myAnimIndex1]->mRootNode
			, Identity);
	} 

	//aTransformsVector.resize(myNumOfBones);
	for (uint i = 0; i < myNumOfBones; i++)
	{
		aTransformsVector[i] = myBoneInfo[i].myFinalTransformation;
	}
}

void CAnimationController::SetBoneTransforms(
	  const std::vector<float>& someBlendingTimes
	, const std::vector<float>& someTicks
	, std::array<aiMatrix4x4, 64>& aTransformsVector)
{
	/*std::vector<const aiNode*> nodes = {};
	for (auto i = 0; i < myScenes.size(); ++i)
		nodes.push_back(myScenes[i]->mRootNode);*/

	aiMatrix4x4 transform;
	InitIdentityM4(transform);
	ReadNodeHeirarchy(
		  someBlendingTimes
		, someTicks
		, myScenes.data()
		, myScenes[0]->mRootNode
		, transform);

		//, nodes.data()
	//aTransformsVector.resize(myNumOfBones);
	for (uint i = 0; i < myNumOfBones; i++)
	{
		aTransformsVector[i] = myBoneInfo[i].myFinalTransformation;
	}
}

void CAnimationController::ReadNodeHeirarchy(
	const aiScene* aScene
	, float tick
	, const aiNode* aNode
	, const aiMatrix4x4& aParentTransform)
{
	aiMatrix4x4 NodeTransformation(aNode->mTransformation);
	const aiNodeAnim* nodeAnimation = FindNodeAnim(aScene->mAnimations[0], aNode->mName.data);
	if (nodeAnimation)
	{
		aiMatrix4x4 ScalingM;
		aiMatrix4x4::Scaling(CalculateInterpolation(nodeAnimation->mScalingKeys, nodeAnimation->mNumScalingKeys, tick).mValue, ScalingM);

		aiMatrix4x4 RotationM;
		InitM4FromM3(RotationM, CalculateInterpolation(nodeAnimation->mRotationKeys, nodeAnimation->mNumRotationKeys, tick).mValue.GetMatrix());

		aiMatrix4x4 TranslationM;
		aiMatrix4x4::Translation(CalculateInterpolation(nodeAnimation->mPositionKeys, nodeAnimation->mNumPositionKeys, tick).mValue, TranslationM);

		// Combine the above transformations	
		NodeTransformation = TranslationM * RotationM * ScalingM;
	}

	aiMatrix4x4 GlobalTransformation = aParentTransform * NodeTransformation;

	if (myBoneMapping.find(aNode->mName.data) != myBoneMapping.end())
	{
		uint BoneIndex = myBoneMapping[aNode->mName.data];
		myBoneInfo[BoneIndex].myFinalTransformation = myGlobalInverseTransform * GlobalTransformation * myBoneInfo[BoneIndex].myBoneOffset;
	}

	for (uint i = 0; i < aNode->mNumChildren; i++)
		ReadNodeHeirarchy(aScene, tick, aNode->mChildren[i], GlobalTransformation);
}

void CAnimationController::ReadNodeHeirarchy(
	const aiScene* aFromScene
	, const aiScene* aToScene
	, float aTickFrom
	, float aTickTo
	, const aiNode* aNodeFrom
	, const aiNode* aNodeTo
	, const aiMatrix4x4& aParentTransform)
{
	aiMatrix4x4 NodeTransformation0(aNodeFrom->mTransformation);
	//aiMatrix4x4 NodeTransformation1(aNodeTo->mTransformation);
	const aiNodeAnim* pNodeAnim0 = FindNodeAnim(aFromScene->mAnimations[0], aNodeFrom->mName.data);
	const aiNodeAnim* pNodeAnim1 = FindNodeAnim(aToScene->mAnimations[0], aNodeTo->mName.data);
	if (pNodeAnim0 && pNodeAnim1)
	{
		aiVector3D ScaleQ = {};
		Assimp::Interpolator<aiVectorKey>()(
			ScaleQ
			, CalculateInterpolation(pNodeAnim0->mScalingKeys, pNodeAnim0->mNumScalingKeys, aTickFrom)
			, CalculateInterpolation(pNodeAnim1->mScalingKeys, pNodeAnim1->mNumScalingKeys, aTickTo)
			, myBlendingTime);

		aiMatrix4x4 ScaleM = {};
		aiMatrix4x4::Scaling(ScaleQ, ScaleM);

		aiQuaternion RotationQ = {};
		Assimp::Interpolator<aiQuatKey>()(
			RotationQ
			, CalculateInterpolation(pNodeAnim0->mRotationKeys, pNodeAnim0->mNumRotationKeys, aTickFrom)
			, CalculateInterpolation(pNodeAnim1->mRotationKeys, pNodeAnim1->mNumRotationKeys, aTickTo)
			, myBlendingTime);
		aiMatrix4x4 RotationM = {};
		InitM4FromM3(RotationM, RotationQ.GetMatrix());

		aiVector3D interpolatedTranslation = {};
		Assimp::Interpolator<aiVectorKey>()(
			interpolatedTranslation
			, CalculateInterpolation(pNodeAnim0->mPositionKeys, pNodeAnim0->mNumPositionKeys, aTickFrom)
			, CalculateInterpolation(pNodeAnim1->mPositionKeys, pNodeAnim1->mNumPositionKeys, aTickTo)
			, myBlendingTime);
		aiMatrix4x4 TranslationM = {};
		aiMatrix4x4::Translation(interpolatedTranslation, TranslationM);

		NodeTransformation0 = TranslationM * RotationM * ScaleM;
	}

	aiMatrix4x4 GlobalTransformation = aParentTransform * NodeTransformation0;
	if (myBoneMapping.find(aNodeFrom->mName.data) != myBoneMapping.end())
	{
		uint BoneIndex = myBoneMapping[aNodeFrom->mName.data];
		myBoneInfo[BoneIndex].myFinalTransformation = myGlobalInverseTransform * GlobalTransformation * myBoneInfo[BoneIndex].myBoneOffset;
	}

	uint n = min(aNodeFrom->mNumChildren, aNodeTo->mNumChildren);
	for (uint i = 0; i < n; i++)
		ReadNodeHeirarchy(aFromScene, aToScene, aTickFrom, aTickTo, aNodeFrom->mChildren[i], aNodeTo->mChildren[i], GlobalTransformation);
}

void CAnimationController::ReadNodeHeirarchy(
	  const std::vector<float>&	someBlendingValues
	, const std::vector<float>&	someTicks
	, const aiScene** someScenes
	, const aiNode* aNode
	//, const uint aDepth
	, const aiMatrix4x4& aParentTransform)
{
	if (aNode == nullptr)//someBlendingValues.size())
		return;

	aiMatrix4x4 NodeTransformation0 = aNode->mTransformation;
	aiVectorKey scaleKeyResult = {};//= CalculateInterpolation(pNodeAnim0->mScalingKeys, pNodeAnim0->mNumScalingKeys, someTicks[0]);
	aiQuatKey rotationKeyResult = {};//= CalculateInterpolation(pNodeAnim0->mRotationKeys, pNodeAnim0->mNumRotationKeys, someTicks[0]);
	aiVectorKey positionKeyResult = {};//= CalculateInterpolation(pNodeAnim0->mPositionKeys, pNodeAnim0->mNumPositionKeys, someTicks[0]);

	size_t one = 1;
	size_t size = someBlendingValues.size();
	for (int i = 1; i < size; ++i)
	{
		//aiMatrix4x4 nodeTransform(someNodes[i - one]->mTransformation);
		const aiNodeAnim* anim = FindNodeAnim(someScenes[i - one]->mAnimations[0], aNode->mName.data);
		if (anim == nullptr)
			continue;
		

		if (someTicks[i - one] < someTicks[i] || someTicks[i] < someTicks[i - 1])
		{

		}

		aiVectorKey scaleKey = CalculateInterpolation(anim->mScalingKeys, anim->mNumScalingKeys, someTicks[i - one]);
		aiVectorKey nextScaleKey = CalculateInterpolation(anim->mScalingKeys, anim->mNumScalingKeys, someTicks[i]);
		aiVector3D scale = {};
		Assimp::Interpolator<aiVectorKey>()(
			scale
			, scaleKey
			, nextScaleKey
			, someBlendingValues[i]);
		aiMatrix4x4 scaleM = {};
		aiMatrix4x4::Scaling(scale, scaleM);


		aiQuatKey rotationKey = CalculateInterpolation(anim->mRotationKeys, anim->mNumRotationKeys, someTicks[i - one]);
		aiQuatKey nextRotationKey = CalculateInterpolation(anim->mRotationKeys, anim->mNumRotationKeys, someTicks[i]);
		aiQuaternion rotation = {};
		Assimp::Interpolator<aiQuatKey>()(
			rotation
			, rotationKey
			, nextRotationKey
			, someBlendingValues[i]);
		aiMatrix4x4 rotationM = {};
		InitM4FromM3(rotationM, rotation.GetMatrix());

		aiVectorKey positionKey = CalculateInterpolation(anim->mPositionKeys, anim->mNumPositionKeys, someTicks[i - one]);
		aiVectorKey nextPositionKey = CalculateInterpolation(anim->mPositionKeys, anim->mNumPositionKeys, someTicks[i]);
		aiVector3D position = {};
		Assimp::Interpolator<aiVectorKey>()(
			position
			, positionKey
			, nextPositionKey
			, someBlendingValues[i]);
		aiMatrix4x4 translationM = {};
		aiMatrix4x4::Translation(position, translationM);
		NodeTransformation0 = translationM * rotationM * scaleM;
	}

	aiMatrix4x4 GlobalTransformation = aParentTransform * NodeTransformation0;

	if (myBoneMapping.find(aNode->mName.data) != myBoneMapping.end())
	{
		uint BoneIndex = myBoneMapping[aNode->mName.data];
		myBoneInfo[BoneIndex].myFinalTransformation = myGlobalInverseTransform * GlobalTransformation * myBoneInfo[BoneIndex].myBoneOffset;
	}

	//uint n = UINT8_MAX;
	//for (size_t i = 0; i < size; ++i)
	//{
	//	if (aNode->mNumChildren < n)
	//		n = aNode->mNumChildren;
	//}

	
	//for (; n > 0; --n)
	//{
	for (unsigned int i = 0; i < size; ++i)
	{
		if (aNode->mNumChildren > i)
		{
			ReadNodeHeirarchy(
				  someBlendingValues
				, someTicks
				, someScenes
				, aNode->mChildren[i]
				, GlobalTransformation);
		}
	}
}
	//	ReadNodeHeirarchy(
	//		  someBlendingValues
	//		, someTicks
	//		, someScenes
	//		, aNode->mChildren[n - 1]
	///*		, aDepth + 1*/
	//		, GlobalTransformation);
	//}


	//for (uint i = 0; i < n; ++i)
	//{
	//	const auto childNodeTest = someNodes[i]->mChildren;
	//	const aiNode** childNode = someNodes[i]->mChildren;
	//
	//	//std::vector<const aiNode*> nodes = {};
	//	//for (auto i = 0; i < myScenes.size(); ++i)
	//	//	nodes.push_back(myScenes[i]->mRootNode);
	//
	//	ReadNodeHeirarchy(
	//		someBlendingValues, 
	//		someTicks, 
	//		someScenes, 
	//		childNodeTest,
	//		GlobalTransformation);
	//}

	//uint n = min(aNodeFrom->mNumChildren, aNodeTo->mNumChildren);

	/*uint n = min(aNodeFrom->mNumChildren, aNodeTo->mNumChildren);
	for (uint i = 0; i < n; i++)
		ReadNodeHeirarchy(someScenes, someTicks, aNodeFrom->mChildren[i], aNodeTo->mChildren[i], GlobalTransformation);*/



void CAnimationController::UpdateAnimationTimes(std::array<SlimMatrix44, 64>& someBones)
{
	float dt = CTimer::Dt();

	myTicks0 = fmodf(myTicks0 + dt, AnimationDurationInSeconds(myAnimIndex0));//duration0 /*- dt*/);
	myTicks1 = fmodf(myTicks1 + dt, AnimationDurationInSeconds(myAnimIndex1));

	std::array<aiMatrix4x4, 64> trans;
	SetBoneTransforms(trans);
	memmove(someBones.data(), &trans[0], (sizeof(float) * 16) * trans.size());//was memcpy
}
const int CAnimationController::AnimationCount() const
{
	return static_cast<uint>(myScenes.size());
}
const float CAnimationController::AnimationDurationInTicks(int anIndex) const
{
	return static_cast<float>(myScenes[anIndex]->mAnimations[0]->mDuration);
}
const int CAnimationController::AnimationIndex(const std::string& aMotionName)
{
	const aiScene* scene = mySceneMap[aMotionName];
	for (int i = 0; i < myScenes.size(); ++i)
		if (scene == myScenes[i])
			return i;

	return 0;
}
const float CAnimationController::AnimationTPS(const std::string& aMotionName)
{
	return static_cast<float>(mySceneMap[aMotionName]->mAnimations[0]->mTicksPerSecond);
}
const float CAnimationController::AnimationTPS(int anIndex) const
{
	return static_cast<float>(myScenes[anIndex]->mAnimations[0]->mTicksPerSecond);
}
const float CAnimationController::AnimationDurationInSeconds(int anIndex) const
{
	return static_cast<float>(myScenes[anIndex]->mAnimations[0]->mDuration / myScenes[anIndex]->mAnimations[0]->mTicksPerSecond);
}
bool CAnimationController::AnimationIndexWithinRange(int anIndex) const
{
	return anIndex == static_cast<int>(myAnimIndex0) || anIndex >= static_cast<int>(myScenes.size());
}
CBlendTree* CAnimationController::AddBlendTree(const char* aName)
{
	myBlendTrees.push_back(CBlendTree(aName, *this));
	return &myBlendTrees.back();
}
std::vector<std::string> CAnimationController::GetMotionNames()
{
	std::vector<std::string> keys;
	for (auto& keyValue : mySceneMap)
		keys.push_back(keyValue.first);

	return keys;
}
void CAnimationController::SaveMotionToMap(const aiScene* aScene, const std::string& aFilePath)
{
	auto lastSlash = aFilePath.find_last_of('/') + 1;
	std::string animName = aFilePath.substr(lastSlash, aFilePath.size() - lastSlash);
	mySceneMap[animName] = aScene;
	myTicks.push_back(0.0f);
}