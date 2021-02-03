#include "stdafx.h"
#include "AnimationController.h"

#include "../ModelLoader/modelExceptionTools.h"

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
{}

CAnimationController::~CAnimationController()
{
// No longer used 2021 02 01
	//for (uint i = 0; i < myImporters.size(); ++i)
	//{
	//	delete myImporters[i];
	//	myImporters[i] = nullptr;
	//}
	//myImporters.clear();

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
	}
	else
	{
		logInfo(importer.GetErrorString());
	}
// Quick test that gave no noticable result 2021 02 01
	//delete myAnimations[myAnim0Index]->mMeshes;
	//myAnimations[myAnim0Index]->mMeshes = nullptr;
	//delete myAnimations[myAnim0Index]->mMaterials;
	//myAnimations[myAnim0Index]->mMaterials = nullptr;
	//delete myAnimations[myAnim0Index]->mTextures;
	//myAnimations[myAnim0Index]->mTextures = nullptr;

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
	if(importer.ReadFile(fileName, aiProcessPreset_TargetRealtime_Quality | aiProcess_ConvertToLeftHanded))
		myAnimations.emplace_back(importer.GetOrphanedScene());
	else
		return false;

	// If the import failed, report it
	if (!myAnimations[myAnim0Index])
	{
		logInfo(importer.GetErrorString());
		return false;
	}

// Quick test that gave no noticable result 2021 02 01
	//delete myAnimations[myAnim0Index]->mMeshes;
	//myAnimations[myAnim0Index]->mMeshes = nullptr;
	//delete myAnimations[myAnim0Index]->mMaterials;
	//myAnimations[myAnim0Index]->mMaterials = nullptr;
	//delete myAnimations[myAnim0Index]->mTextures;
	//myAnimations[myAnim0Index]->mTextures = nullptr;

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

		for (uint j = 0; j < aMesh->mBones[i]->mNumWeights; j++)
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
		// Interpolate scaling and generate scaling transformation matrix
		aiVector3D Scaling;
		CalcInterpolatedScaling(Scaling, animationTime, nodeAnimation);
		aiMatrix4x4 ScalingM;
		aiMatrix4x4::Scaling(Scaling, ScalingM);

		// Interpolate rotation and generate rotation transformation matrix
		aiQuaternion RotationQ;
		CalcInterpolatedRotation(RotationQ, animationTime, nodeAnimation);
		aiMatrix4x4 RotationM;
		InitM4FromM3(RotationM, RotationQ.GetMatrix());

		// Interpolate translation and generate translation transformation matrix
		aiVector3D Translation;
		{
			float timeStop(aStopAnimAtLevel <= 0 ? animationTime : 0.f);
			CalcInterpolatedPosition(Translation, timeStop, nodeAnimation);
		}
		aiMatrix4x4 TranslationM;
		aiMatrix4x4::Translation(Translation, TranslationM);

		// Combine the above transformations
		NodeTransformation = TranslationM * RotationM * ScalingM;//Original, intended. But now blob-like
	}
	aStopAnimAtLevel--;

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
	float time0(anAnimationTimeFrom);
	float time1(anAnimationTimeTo);

	std::string NodeName0(aStartNodeFrom->mName.data);
	std::string NodeName1(aStartNodeTo->mName.data);
// Commented cause it was annoying when testing 2021 01 25
	//assert(NodeName0 == NodeName1);

	const aiAnimation* pAnimation0 = aFromScene->mAnimations[0];
	const aiAnimation* pAnimation1 = aToScene->mAnimations[0];

	aiMatrix4x4 NodeTransformation0(aStartNodeFrom->mTransformation);
	aiMatrix4x4 NodeTransformation1(aStartNodeTo->mTransformation);

	const aiNodeAnim* pNodeAnim0 = FindNodeAnim(pAnimation0, NodeName0);
	const aiNodeAnim* pNodeAnim1 = FindNodeAnim(pAnimation1, NodeName0);

	if (pNodeAnim0 && pNodeAnim1)
	{
		// Interpolate scaling and generate scaling transformation matrix
		aiVector3D Scaling0;
		CalcInterpolatedScaling(Scaling0, time0, pNodeAnim0);
		aiVector3D Scaling1;
		CalcInterpolatedScaling(Scaling1, time1, pNodeAnim1);
		aiMatrix4x4 ScalingM;
		aiMatrix4x4::Scaling(Scaling0 * myBlendingTime + Scaling1 * (1.f - myBlendingTime), ScalingM);

		// Interpolate rotation and generate rotation transformation matrix
		aiQuaternion RotationQ0;
		CalcInterpolatedRotation(RotationQ0, time0, pNodeAnim0);
		aiQuaternion RotationQ1;
		CalcInterpolatedRotation(RotationQ1, time1, pNodeAnim1);
		aiMatrix4x4 RotationM;
		aiQuaternion RotationQ;
		aiQuaternion::Interpolate(RotationQ, RotationQ1, RotationQ0, myBlendingTime);
		InitM4FromM3(RotationM, RotationQ.GetMatrix());

		// Interpolate translation and generate translation transformation matrix
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
		NodeTransformation0 = TranslationM * RotationM * ScalingM;
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

void CAnimationController::SetBoneTransforms(std::vector<aiMatrix4x4>& aTransformsVector)
{
	aiMatrix4x4 Identity;
	InitIdentityM4(Identity);

	if (myBlendingTime > 0.f)
	{
		// Ticks == Frames
		float ticksPerSecond = static_cast<float>(myAnimations[myAnim0Index]->mAnimations[0]->mTicksPerSecond);
		ticksPerSecond = (ticksPerSecond != 0) ? ticksPerSecond : TEMP_FRAMES_PER_SECOND;
		
		float timeInTicks = myAnimationTime0 * ticksPerSecond;
		float animationTime0 = fmodf(timeInTicks, static_cast<float>(myAnimations[myAnim0Index]->mAnimations[0]->mDuration));
		
		ticksPerSecond = static_cast<float>(myAnimations[myAnim1Index]->mAnimations[0]->mTicksPerSecond);
		ticksPerSecond = (ticksPerSecond != 0) ? ticksPerSecond : TEMP_FRAMES_PER_SECOND;
		
		timeInTicks = myAnimationTime1 * ticksPerSecond;
		float animationTime1 = fmodf(timeInTicks, static_cast<float>(myAnimations[myAnim1Index]->mAnimations[0]->mDuration));
		
		ReadNodeHeirarchy(	myAnimations[myAnim1Index], myAnimations[myAnim0Index]
						  , animationTime0, animationTime1
						  , myAnimations[myAnim1Index]->mRootNode, myAnimations[myAnim0Index]->mRootNode
						  , Identity, 2);//stopAnimLevel=2
	}
	else
	{
		float ticksPerSecond = static_cast<float>(myAnimations[myAnim0Index]->mAnimations[0]->mTicksPerSecond);
		ticksPerSecond = (ticksPerSecond != 0) ? ticksPerSecond : TEMP_FRAMES_PER_SECOND;

		float timeInTicks = myAnimationTime0 * ticksPerSecond /** 40.0f*//*ticksPerSecond*/ ;
		//float timeInTicks = myAnimationTime0;//Suuuper slow
		//float timeInTicks = (myAnimationTime0 / CTimer::Dt()) * ticksPerSecond;//Super speed //(Update is using dt = CTimer::Dt())
		//float timeInTicks = myAnimationTime0 * (ticksPerSecond / CTimer::Dt());/Super speed //(Update is using dt = CTimer::Dt())
		//float timeInTicks = myAnimationTime0 / ticksPerSecond;// Turtle speed :) //(Update is using dt = CTimer::Dt())
		
		float duration = static_cast<float>(myAnimations[myAnim0Index]->mAnimations[0]->mDuration);
		//float duration = 40.0f;
		float animationTime = fmodf(timeInTicks, duration);// In SP6 Project: mDuration is duration in frames. I.e 34frames for Undead Idle  @ 24fps
		// 40.0f which is the actual nr of frames the animation has does not move the model, i.e the animation stands still
		// Seems to be cause FindScaling/Rotation/Position() are returning the value of the first node they find which had 41.something mTime. if(AnimationTime < node.key[i]->mTime) return i
		std::cout << timeInTicks << " " << duration << "  " << animationTime << std::endl;

		ReadNodeHeirarchy(myAnimations[myAnim0Index], animationTime, myAnimations[myAnim0Index]->mRootNode, Identity, 2);//stopAnimLevel=2
	}

	aTransformsVector.resize(myNumOfBones);

	for (uint i = 0; i < myNumOfBones; i++)
	{
		aTransformsVector[i] = myBoneInfo[i].myFinalTransformation;
	}
}

void CAnimationController::UpdateAnimationTimeConstant(const float aStep)
{
	float dt = aStep;// Close to good (right)

	myAnimationTime0 += dt;
	if (myBlendingTime > 0.f)
	{
		myBlendingTime -= dt * myBlendingTimeMul;
		if (myBlendingTime <= 0.f)
		{
			myAnimationTime0 = myAnimationTime1;
		}
		if (myUpdateBoth)
		{
			myAnimationTime1 += dt;
		}
	}
	else
	{
		myAnimationTime1 += dt;
	}

	if (myTemporary)// If the animation was temporary, return to the previous animation after the playtime is over
	{
		myPlayTime -= dt;
		if (myPlayTime <= 0.f)
		{
			myTemporary = false;
			BlendToAnimation(myAnim1Index);
		}
	}
}

void CAnimationController::UpdateAnimationTimes()
{
	//float dt = CTimer::Dt() * ((float)myAnimations[myAnim0Index]->mAnimations[0]->mDuration / TEMP_FRAMES_PER_SECOND);
	//float dt = CTimer::Dt() * TEMP_FRAMES_PER_SECOND);
	float dt = CTimer::Dt() * 12.0f;// Close to good (right)

	myAnimationTime0 += dt;
	if (myBlendingTime > 0.f)
	{
		myBlendingTime -= dt * myBlendingTimeMul;
		if (myBlendingTime <= 0.f)
		{
			myAnimationTime0 = myAnimationTime1;
		}
		if (myUpdateBoth)
		{
			myAnimationTime1 += dt;
		}
	}
	else
	{
		myAnimationTime1 += dt;
	}

	if (myTemporary)// If the animation was temporary, return to the previous animation after the playtime is over
	{
		myPlayTime -= dt;
		if (myPlayTime <= 0.f)
		{
			myTemporary = false;
			BlendToAnimation(myAnim1Index);
		}
	}
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


/*
/////////////////////////////////////////////////////////////////////////////////////////
// BACKUP OF OLD IMPORTING FUNCTIONS (using std::vector<Assimp::Importer*> myImportes //
///////////////////////////////////////////////////////////////////////////////////////

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

myAnim0Index = static_cast<int>(myImporters.size());
myImporters.emplace_back(new Assimp::Importer);

if (ModelExceptionTools::IsDestructibleModel(anFBXFilePath))
{
int qualityFlags = aiProcessPreset_TargetRealtime_Quality;
myAnimations.push_back(myImporters[myAnim0Index]->ReadFile(anFBXFilePath, (qualityFlags ^= (int)aiProcess_JoinIdenticalVertices) | aiProcess_ConvertToLeftHanded));
}
else
{
myAnimations.push_back(myImporters[myAnim0Index]->ReadFile(anFBXFilePath, aiProcessPreset_TargetRealtime_Quality | aiProcess_ConvertToLeftHanded));
}

bool ret = false;
// If the import failed, report it
if (myAnimations[myAnim0Index])
{
myGlobalInverseTransform = myAnimations[myAnim0Index]->mRootNode->mTransformation;
myGlobalInverseTransform.Inverse();
ret = InitFromScene(myAnimations[myAnim0Index]);
// Now we can access the file's contents.
logInfo("Import of _curScene " + anFBXFilePath + " succeeded.");
}
else
{
logInfo(myImporters[myAnim0Index]->GetErrorString());
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

myAnim0Index = static_cast<int>(myImporters.size());
myImporters.emplace_back(new Assimp::Importer);
myAnimations.emplace_back(myImporters[myAnim0Index]->ReadFile(fileName, aiProcessPreset_TargetRealtime_Quality | aiProcess_ConvertToLeftHanded));

// If the import failed, report it
if (!myAnimations[myAnim0Index])
{
logInfo(myImporters[myAnim0Index]->GetErrorString());
return false;
}
return true;
}
*/