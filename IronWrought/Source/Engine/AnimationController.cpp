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
	if (importer.ReadFile(anFBXFilePath, aiProcessPreset_TargetRealtime_Quality | aiProcess_ConvertToLeftHanded))
	{
		//scene->mAnimations = new aiAnimation[someAnimationPaths.size()];
		myScenes.emplace_back(importer.GetOrphanedScene());
	}
	else
		return false;

	bool ret = false;
	// If the import failed, report it
	if (myScenes[myAnimIndex0])
	{
		myGlobalInverseTransform = myScenes[myAnimIndex0]->mRootNode->mTransformation;
		myGlobalInverseTransform.Inverse();
		ret = InitFromScene(myScenes[myAnimIndex0]);

		// Now we can access the file's contents.
		//logInfo("Import of _curScene " + anFBXFilePath + " succeeded.");

		//if (myAnimations[myAnim0Index]->mNumAnimations <= 0)
		//{
		//	ENGINE_ERROR_BOOL_MESSAGE(false , std::string("Animation could not be loaded: " + anFBXFilePath).c_str());
		//	return false;
		//}
	}
	else
	{
		//logInfo(importer.GetErrorString());
	}

	// We're done. Everything will be cleaned up by the importer destructor
	return ret;
}
bool CAnimationController::ImportAnimation(const std::string& fileName)
{
	// Check if file exists
	std::ifstream fin(fileName.c_str());
	if (!fin.fail())
		fin.close();
	else
	{
		MessageBoxA(NULL, ("Couldn't open file: " + fileName).c_str(), "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return false;
	}

	Assimp::Importer importer;

	myAnimIndex0 = static_cast<int>(myScenes.size());
	if (importer.ReadFile(fileName, aiProcessPreset_TargetRealtime_Quality | aiProcess_ConvertToLeftHanded))
		myScenes.push_back(importer.GetOrphanedScene());
	else
		return false;

	myAnimationClipNames.push_back(fileName);

	// If the import failed, report it
	if (!myScenes[myAnimIndex0])
	{
		//logInfo(importer.GetErrorString());
		return false;
	}

	if (myScenes[myAnimIndex0]->mNumAnimations <= 0)
	{
		ENGINE_ERROR_BOOL_MESSAGE(false, std::string("Animation could not be loaded: " + fileName).c_str());
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
		//NodeTransformation1 = TranslationM * RotationM * ScaleM;
	}

	//aiMatrix4x4 NodeTransformationBlended = {};
	//Assimp::Interpolator<aiMatrix4x4>()(NodeTransformationBlended, NodeTransformation0, NodeTransformation1, myBlendingTime);
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

const float CAnimationController::AnimationTPS(int anIndex) const
{
	return static_cast<float>(myScenes[anIndex]->mAnimations[0]->mTicksPerSecond);
}

const float CAnimationController::AnimationDurationInSeconds(int anIndex) const
{
	if (AnimationIndexWithinRange(anIndex))
		return static_cast<float>(myScenes[anIndex]->mAnimations[0]->mDuration / myScenes[anIndex]->mAnimations[0]->mTicksPerSecond);
	return 0.0f;
}

bool CAnimationController::AnimationIndexWithinRange(int anIndex) const
{
	return anIndex == static_cast<int>(myAnimIndex0) || anIndex >= static_cast<int>(myScenes.size());
}

void CAnimationController::UpdateAnimationTimeFrames()
{


	//if (myBlendingTime > 0.f)
	//{
	//	myBlendingTime -= dt * myBlendingTimeMul;
	//	if (myBlendingTime <= 0.f)
	//	{
	//		myAnimationTime0 = myAnimationTime1;
	//	}
	//	if (myUpdateBoth)
	//	{
	//		myAnimationTime1 += dt;
	//	}
	//}
	//else
	//{
	//	myAnimationTime1 += dt;
	//}

	//if (myTemporary)// If the animation was temporary, return to the previous animation after the playtime is over
	//{
	//	myPlayTime -= dt;
	//	if (myPlayTime <= 0.f)
	//	{
	//		myTemporary = false;
	//		BlendToAnimation(myAnim1Index);
	//	}
	//}
}


/*
	FIX FOR previous assimp mDuration issues :D :O :(

void CAnimationController::UpdateAnimationTimeMilliseconds()
{
	myAnimationTime0 = CTimer::Time() * f_milliseconds;
	if (myBlendingTime > 0.f)
	{
		myBlendingTime -= CTimer::Dt() * f_milliseconds * myBlendingTimeMul;
		if (myBlendingTime <= 0.f)
		{
			myAnimationTime0 = myAnimationTime1;
		}
		if (myUpdateBoth)
		{
			myAnimationTime1 = CTimer::Time() * f_milliseconds;
		}
	}
	else
	{
		myAnimationTime1 = CTimer::Time() * f_milliseconds;
	}

	if (myTemporary)// If the animation was temporary, return to the previous animation after the playtime is over
	{
		myPlayTime -= CTimer::Dt() * f_milliseconds;// Not verified if it works with ANIMATION_DURATION_IN_MILLISECONDS
		if (myPlayTime <= 0.f)
		{
			myTemporary = false;
			BlendToAnimation(myAnim1Index);
		}
	}
}

void CAnimationController::ConvertAnimationTimesToFrames(aiScene* aScene)
{
	const float convertToFrame = (ANIMATED_AT_FRAMES_PER_SECOND / f_milliseconds);
	aiAnimation* animation = aScene->mAnimations[0];

	animation->mDuration *= convertToFrame;
	for (uint i = 0; i < animation->mNumChannels; ++i)
	{
		for (uint s = 0; s < animation->mChannels[i]->mNumScalingKeys; ++s)
		{
			animation->mChannels[i]->mScalingKeys[s].mTime *= convertToFrame;
		}
		for (uint r = 0; r < animation->mChannels[i]->mNumRotationKeys; ++r)
		{
			animation->mChannels[i]->mRotationKeys[r].mTime *= convertToFrame;
		}
		for (uint p = 0; p < animation->mChannels[i]->mNumPositionKeys; ++p)
		{
			animation->mChannels[i]->mPositionKeys[p].mTime *= convertToFrame;
		}
	}
}
*/