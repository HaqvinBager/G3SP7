#include "stdafx.h"
#include "AnimationController.h"
#include "Timer.h"
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
	, myAnim0Index(0)
	, myAnim1Index(0)
	, myRotation(0.f)
	, myBlendingTime(0.f)
	, myBlendingTimeMul(1.f)
	, myUpdateBoth(true)
	, myTemporary(false)
	, myPlayTime(0.f)
	, myAnimationTime1(0)
	, myAnimationTime0(0)
{
}

CAnimationController::~CAnimationController()
{
	for (size_t i = 0; i < myAnimations.size(); ++i)
	{
		delete myAnimations[i];
		myAnimations[i] = nullptr;
	}
	myAnimations.clear();
}

bool CAnimationController::ImportRig(const std::string& anFBXFilePath)
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

	myAnim0Index = static_cast<int>(myAnimations.size());

	Assimp::Importer importer;
	if (importer.ReadFile(anFBXFilePath, aiProcessPreset_TargetRealtime_Quality | aiProcess_ConvertToLeftHanded))
		myAnimations.emplace_back(importer.GetOrphanedScene());
	else
		return false;

	bool ret = false;
	// If the import failed, report it
	if (myAnimations[myAnim0Index])
	{
		myGlobalInverseTransform = myAnimations[myAnim0Index]->mRootNode->mTransformation;
		myGlobalInverseTransform.Inverse();
		ret = InitFromScene(myAnimations[myAnim0Index]);
		// Now we can access the file's contents.
		logInfo("Import of _curScene " + anFBXFilePath + " succeeded.");

		//if (myAnimations[myAnim0Index]->mNumAnimations <= 0)
		//{
		//	ENGINE_ERROR_BOOL_MESSAGE(false , std::string("Animation could not be loaded: " + anFBXFilePath).c_str());
		//	return false;
		//}
	}
	else
	{
		logInfo(importer.GetErrorString());
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

	myAnim0Index = static_cast<int>(myAnimations.size());
	if (importer.ReadFile(fileName, aiProcessPreset_TargetRealtime_Quality | aiProcess_ConvertToLeftHanded))
		myAnimations.push_back(importer.GetOrphanedScene());
	else
		return false;

	myAnimationClipNames.push_back(fileName);

	// If the import failed, report it
	if (!myAnimations[myAnim0Index])
	{
		logInfo(importer.GetErrorString());
		return false;
	}

	if (myAnimations[myAnim0Index]->mNumAnimations <= 0)
	{
		ENGINE_ERROR_BOOL_MESSAGE(false, std::string("Animation could not be loaded: " + fileName).c_str());
		return false;
	}

	return true;
}

bool CAnimationController::InitFromScene(const aiScene* pScene)
{
	myAnimationTime0 = 0.0f;
	myAnimationTime1 = 0.0f;

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

void CAnimationController::ReadNodeHeirarchy(
	const aiScene* aScene, float anAnimationTime, const aiNode* aNode
	, const aiMatrix4x4& aParentTransform, int aStopAnimAtLevel)
{
	float animationTime(anAnimationTime);
	std::string nodeName(aNode->mName.data);
	const aiAnimation* animation = aScene->mAnimations[0];
	aiMatrix4x4 NodeTransformation(aNode->mTransformation);
	const aiNodeAnim* nodeAnimation = FindNodeAnim(animation, nodeName);

	if (nodeAnimation)
	{
		// Interpolate scaling and generate scaling transformation lerpMatrixA
		//aiVector3D Scaling;
		//CalcInterpolatedScaling(Scaling, animationTime, nodeAnimation);
		//aiMatrix4x4 ScalingM;
		//aiMatrix4x4::Scaling(Scaling, ScalingM);

		// Interpolate rotation and generate rotation transformation lerpMatrixA
		aiQuaternion RotationQ;
		CalcInterpolatedRotation(RotationQ, animationTime, nodeAnimation);
		aiMatrix4x4 RotationM;
		InitM4FromM3(RotationM, RotationQ.GetMatrix());

		// Interpolate translation and generate translation transformation lerpMatrixA
		aiVector3D Translation;
		{
			float timeStop(aStopAnimAtLevel <= 0 ? animationTime : 0.f);
			CalcInterpolatedPosition(Translation, timeStop, nodeAnimation);
		}
		aiMatrix4x4 TranslationM;
		aiMatrix4x4::Translation(Translation, TranslationM);

		// Combine the above transformations
		NodeTransformation = TranslationM * RotationM /** ScalingM*/;
	}
	aStopAnimAtLevel--;


	// GLobalTransformation is the joints animation for this fram. Multiply with the original joint orientation.
	aiMatrix4x4 GlobalTransformation = aParentTransform * NodeTransformation;

	if (myBoneMapping.find(nodeName) != myBoneMapping.end())
	{
		uint BoneIndex = myBoneMapping[nodeName];
		myBoneInfo[BoneIndex].myFinalTransformation = myGlobalInverseTransform * GlobalTransformation * myBoneInfo[BoneIndex].myBoneOffset;

	}

	for (uint i = 0; i < aNode->mNumChildren; i++)
	{
		ReadNodeHeirarchy(aScene, anAnimationTime, aNode->mChildren[i], GlobalTransformation, aStopAnimAtLevel);
	}
}

void CAnimationController::ReadNodeHeirarchy(
	const aiScene* aFromScene, const aiScene* aToScene, float anAnimationTimeFrom
	, float anAnimationTimeTo, const aiNode* aStartNodeFrom, const aiNode* aStartNodeTo
	, const aiMatrix4x4& aParentTransform, int aStopAnimAtLevel)
{
	const aiAnimation* pAnimation0 = aFromScene->mAnimations[0];
	const aiAnimation* pAnimation1 = aToScene->mAnimations[0];

	//float time0 = Remap(0.0f, 1.0f, 0.0f, static_cast<float>(pAnimation0->mDuration), anAnimationTimeFrom);
	//float time1 = Remap(0.0f, 1.0f, 0.0f, static_cast<float>(pAnimation1->mDuration), anAnimationTimeTo);

	//std::cout << anAnimationTimeFrom << std::endl;

	float time0(anAnimationTimeFrom);
	float time1(anAnimationTimeTo);

	std::string NodeName0(aStartNodeFrom->mName.data);
	std::string NodeName1(aStartNodeTo->mName.data);
	// Commented cause it was annoying when testing 2021 01 25
		//assert(NodeName0 == NodeName1);



	aiMatrix4x4 NodeTransformation0(aStartNodeFrom->mTransformation);
	aiMatrix4x4 NodeTransformation1(aStartNodeTo->mTransformation);

	const aiNodeAnim* pNodeAnim0 = FindNodeAnim(pAnimation0, NodeName0);
	const aiNodeAnim* pNodeAnim1 = FindNodeAnim(pAnimation1, NodeName0);


	if (pNodeAnim0 && pNodeAnim1)
	{
			//aiMatrix4x4 interpolatedMatrix = CalculateInterpolation(pNodeAnim0, pNodeAnim1);	


		// Interpolate scaling and generate scaling transformation lerpMatrixA
		//aiVector3D Scaling0;
		//CalcInterpolatedScaling(Scaling0, time0, pNodeAnim0);
		//aiVector3D Scaling1;
		//CalcInterpolatedScaling(Scaling1, time1, pNodeAnim1);
		//aiMatrix4x4 ScalingM;
		//aiMatrix4x4::Scaling(Scaling0 * myBlendingTime + Scaling1 * (1.f - myBlendingTime), ScalingM);

		// Interpolate rotation and generate rotation transformation lerpMatrixA
		aiQuaternion RotationQ0;
		CalcInterpolatedRotation(RotationQ0, time0, pNodeAnim0);
		aiQuaternion RotationQ1;
		CalcInterpolatedRotation(RotationQ1, time1, pNodeAnim1);
		aiMatrix4x4 RotationM;
		aiQuaternion RotationQ;
		aiQuaternion::Interpolate(RotationQ, RotationQ0, RotationQ1, myBlendingTime);
		InitM4FromM3(RotationM, RotationQ.GetMatrix());

		// Interpolate translation and generate translation transformation lerpMatrixA
		aiVector3D Translation0;
		{
			float time(aStopAnimAtLevel <= 0 ? anAnimationTimeFrom : 0.f);
			CalcInterpolatedPosition(Translation0, time, pNodeAnim0);
		}
		aiVector3D Translation1;
		{
			float time(aStopAnimAtLevel <= 0 ? anAnimationTimeTo : 0.f);
			CalcInterpolatedPosition(Translation1, time, pNodeAnim1);
		}
		aiMatrix4x4 TranslationM;
		aiMatrix4x4::Translation(Translation0 * myBlendingTime + Translation1 * (1.f - myBlendingTime), TranslationM);

		// Combine the above transformations
		NodeTransformation0 = TranslationM * RotationM;// *ScalingM;
	}

	aStopAnimAtLevel--;

	aiMatrix4x4 GlobalTransformation = aParentTransform * NodeTransformation0;

	if (myBoneMapping.find(NodeName0) != myBoneMapping.end())
	{
		uint BoneIndex = myBoneMapping[NodeName0];
		myBoneInfo[BoneIndex].myFinalTransformation = myGlobalInverseTransform * GlobalTransformation * myBoneInfo[BoneIndex].myBoneOffset;
	}

	uint n = min(aStartNodeFrom->mNumChildren, aStartNodeTo->mNumChildren);
	for (uint i = 0; i < n; i++)
	{
		ReadNodeHeirarchy(aFromScene, aToScene, anAnimationTimeFrom, anAnimationTimeTo, aStartNodeFrom->mChildren[i], aStartNodeTo->mChildren[i], GlobalTransformation, aStopAnimAtLevel);
	}
}

//void CAnimationController::Curve(const aiVector3D& a, const aiVector3D& b, const aiVector3D c, float t)
//{
//	
//}

void CAnimationController::Curve(std::vector<aiVector3D>& /*abc*/, std::vector<const aiNodeAnim*>& /*pNodeAnims*/, float /*t*/)
{

	/*CalcInterpolatedScaling(abc[0], t, pNodeAnims[0]);
	CalcInterpolatedScaling(abc[1], t, pNodeAnims[1]);
	CalcInterpolatedScaling(abc[2], t, pNodeAnims[2]);
	aiMatrix4x4 lerpMatrixA = {};
	aiMatrix4x4::Scaling((abc[0] * t + abc[1] * (1.0f - t)), lerpMatrixA);

	aiMatrix4x4 lerpMatrixB = {};
	aiMatrix4x4::Scaling((abc[1] * t + abc[2] * (1.0f - t)), lerpMatrixB);*/
}

void CAnimationController::SetBoneTransforms(std::vector<aiMatrix4x4>& aTransformsVector)
{
	aiMatrix4x4 Identity;
	InitIdentityM4(Identity);

	if (myAnim0Index == myAnim1Index)
	{
		float ticksPerSecond = static_cast<float>(myAnimations[myAnim0Index]->mAnimations[0]->mTicksPerSecond);
		ticksPerSecond = (ticksPerSecond != 0) ? ticksPerSecond : ANIMATED_AT_FRAMES_PER_SECOND;

		float timeInTicks = myAnimationTime0 * ticksPerSecond;

		float duration = static_cast<float>(myAnimations[myAnim0Index]->mAnimations[0]->mDuration);
		float animationTime = duration < (timeInTicks + (ticksPerSecond * 0.1f)) ? 0.0f : timeInTicks;

		ReadNodeHeirarchy(myAnimations[myAnim0Index], animationTime, myAnimations[myAnim0Index]->mRootNode, Identity, 2);//stopAnimLevel=2
	}
	else
	{
		// Ticks == Frames
		float ticksPerSecond = static_cast<float>(myAnimations[myAnim0Index]->mAnimations[0]->mTicksPerSecond);
		ticksPerSecond = (ticksPerSecond != 0) ? ticksPerSecond : ANIMATED_AT_FRAMES_PER_SECOND;

		float timeInTicks = myAnimationTime0 * ticksPerSecond;

		float duration0 = static_cast<float>(myAnimations[myAnim0Index]->mAnimations[0]->mDuration);
		float animationTime0 = duration0 < (timeInTicks + (ticksPerSecond * 0.1f)) ? 0.0f : timeInTicks;
		//fmodf(timeInTicks, static_cast<float>(myAnimations[myAnim0Index]->mAnimations[0]->mDuration));

		ticksPerSecond = static_cast<float>(myAnimations[myAnim1Index]->mAnimations[0]->mTicksPerSecond);
		ticksPerSecond = (ticksPerSecond != 0) ? ticksPerSecond : ANIMATED_AT_FRAMES_PER_SECOND;

		timeInTicks = myAnimationTime1 * ticksPerSecond;

		float duration1 = static_cast<float>(myAnimations[myAnim1Index]->mAnimations[0]->mDuration);
		float animationTime1 = duration1 < (timeInTicks + (ticksPerSecond * 0.1f)) ? 0.0f : timeInTicks;
		//fmodf(timeInTicks, static_cast<float>(myAnimations[myAnim1Index]->mAnimations[0]->mDuration));

		ReadNodeHeirarchy(
			myAnimations[myAnim0Index]
			, myAnimations[myAnim1Index]
			, animationTime0
			, animationTime1
			, myAnimations[myAnim0Index]->mRootNode
			, myAnimations[myAnim1Index]->mRootNode
			, Identity, 2);
	}


	//if (myBlendingTime > 0.f)
	//{
	//	// Ticks == Frames
	//	float ticksPerSecond = static_cast<float>(myAnimations[myAnim0Index]->mAnimations[0]->mTicksPerSecond);
	//	ticksPerSecond = (ticksPerSecond != 0) ? ticksPerSecond : ANIMATED_AT_FRAMES_PER_SECOND;
	//	
	//	float timeInTicks = myAnimationTime0 * ticksPerSecond;

	//	float animationTime0 = fmodf(timeInTicks, static_cast<float>(myAnimations[myAnim0Index]->mAnimations[0]->mDuration));
	//	
	//	ticksPerSecond = static_cast<float>(myAnimations[myAnim1Index]->mAnimations[0]->mTicksPerSecond);
	//	ticksPerSecond = (ticksPerSecond != 0) ? ticksPerSecond : ANIMATED_AT_FRAMES_PER_SECOND;
	//	
	//	timeInTicks = myAnimationTime1 * ticksPerSecond;

	//	float animationTime1 = fmodf(timeInTicks, static_cast<float>(myAnimations[myAnim1Index]->mAnimations[0]->mDuration));

	//	

	//	ReadNodeHeirarchy(	myAnimations[myAnim0Index], myAnimations[myAnim1Index]
	//					  , animationTime0, animationTime1
	//					  , myAnimations[myAnim0Index]->mRootNode, myAnimations[myAnim1Index]->mRootNode
	//					  , Identity, 2);//stopAnimLevel=2
	//}
	//else
	//{
	//	float ticksPerSecond = static_cast<float>(myAnimations[myAnim0Index]->mAnimations[0]->mTicksPerSecond);
	//	ticksPerSecond = (ticksPerSecond != 0) ? ticksPerSecond : ANIMATED_AT_FRAMES_PER_SECOND;

	//	float timeInTicks = myAnimationTime0 * ticksPerSecond;

	//	float duration = static_cast<float>(myAnimations[myAnim0Index]->mAnimations[0]->mDuration);
	//	float animationTime = fmodf(timeInTicks, duration);

	//	ReadNodeHeirarchy(myAnimations[myAnim0Index], animationTime, myAnimations[myAnim0Index]->mRootNode, Identity, 2);//stopAnimLevel=2
	//}

	aTransformsVector.resize(myNumOfBones);

	for (uint i = 0; i < myNumOfBones; i++)
	{
		aTransformsVector[i] = myBoneInfo[i].myFinalTransformation;
	}
}
//#ifdef _DEBUG
//void CAnimationController::UpdateAnimationTimeConstant(const float aStep)
//{
//	float dt = aStep;// Close to good (right)
//
//	myAnimationTime0 += dt;
//	if (myBlendingTime > 0.f)
//	{
//		myBlendingTime -= dt * myBlendingTimeMul;
//		if (myBlendingTime <= 0.f)
//		{
//			myAnimationTime0 = myAnimationTime1;
//		}
//		if (myUpdateBoth)
//		{
//			myAnimationTime1 += dt;
//		}
//	}
//	else
//	{
//		myAnimationTime1 += dt;
//	}
//
//	if (myTemporary)// If the animation was temporary, return to the previous animation after the playtime is over
//	{
//		myPlayTime -= dt;
//		if (myPlayTime <= 0.f)
//		{
//			myTemporary = false;
//			BlendToAnimation(myAnim1Index);
//		}
//	}
//}
//#endif
void CAnimationController::UpdateAnimationTimes()
{
	float dt = CTimer::Dt();

	myAnimationTime0 += dt; //fmodf(myAnimationTime0 + dt, static_cast<float>(myAnimations[myAnim0Index]->mAnimations[0]->mDuration));
	myAnimationTime1 += dt;  //fmodf(myAnimationTime1 + dt, static_cast<float>(myAnimations[myAnim1Index]->mAnimations[0]->mDuration));

	//myAnimationTime0 += dt;
	//myAnimationTime1 += dt;

	//UpdateAnimationTimeFrames();
}

void CAnimationController::BlendToAnimation(uint anAnimationIndex, bool anUpdateBoth, float aBlendDuration, bool aTemporary, float aTime)
{
	if (AnimationIndexWithinRange(anAnimationIndex))
		return;

	myAnim1Index = myAnim0Index;
	myAnim0Index = anAnimationIndex;
	myBlendingTime = aBlendDuration;
	myBlendingTimeMul = aBlendDuration > 0.0f ? 1.0f / aBlendDuration : 1.0f;
	myAnimationTime1 = 0.f;
	myUpdateBoth = anUpdateBoth;
	myTemporary = aTemporary;
	myPlayTime = aTime;
}

bool CAnimationController::SetBlendTime(float aTime)
{
	myBlendingTime = aTime;
	return true;
}

uint CAnimationController::GetMaxIndex()
{
	return static_cast<uint>(myAnimations.size());
}

bool CAnimationController::IsDoneBlending()
{
	return myBlendingTime <= 0.0f;
}

const float CAnimationController::AnimationDuration(uint anIndex)
{
	if (AnimationIndexWithinRange(anIndex))
		return (float)myAnimations[anIndex]->mAnimations[0]->mDuration;
	return 0.0f;
}

const float CAnimationController::Animation0Duration()
{
	return (float)myAnimations[myAnim0Index]->mAnimations[0]->mDuration;
}

const float CAnimationController::Animation1Duration()
{
	return (float)myAnimations[myAnim1Index]->mAnimations[0]->mDuration;
}

bool CAnimationController::AnimationIndexWithinRange(uint anIndex)
{
	return anIndex == myAnim0Index || anIndex >= static_cast<uint>(myAnimations.size());
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