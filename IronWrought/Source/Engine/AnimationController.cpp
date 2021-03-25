#include "stdafx.h"
#include "AnimationController.h"
#include "Timer.h"
#include <AssImp\anim.h>
#include <math.h>
#include "EngineException.h"
#include <filesystem>




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

void CAnimationController::ImportSkeleton(const std::string& aSkeletonFBXPath)
{
	ENGINE_BOOL_POPUP(aSkeletonFBXPath.length(), "Imported Skeleton path was empty");

	// Check if file exists
	std::ifstream fIn(aSkeletonFBXPath.c_str());
	ENGINE_BOOL_POPUP(!fIn.fail(), "Failed to read Imported Skeleton %s", aSkeletonFBXPath.c_str());
	fIn.close();
	Assimp::Importer importer = {};
	importer.ReadFile(aSkeletonFBXPath.c_str(), aiProcessPreset_TargetRealtime_Quality | aiProcess_ConvertToLeftHanded);	

//Next Step is to Follow the Book proper!
//Best idea is probably to make sure that i understand how the Import Really works It's at least more than 1 step
	//1 -> Load Assimp::aiScene*
	//2 -> Init Bones 
	//3 -> seems like it's here the vertex-id's are corrolated to each bone!
	//Maybe i don't need to refactor the import things!
	//Only reason to do so is if the book-implementation requires it! : )
		 //Remember -> The book can bring me all the way to Inverse Kinematics! Hopefully...!



	/*const aiScene* scene = importer.GetScene();
	const aiNode* rootNode = scene->mRootNode;*/
	


	//unsigned int boneCount = scene->mMeshes[0]->mNumBones;
	//mySkeleton = std::make_unique<Skeleton>();
	//mySkeleton->myJoints.reserve(boneCount);
	//std::unordered_map<size_t, unsigned int> boneIDMap;
	//for (unsigned int i = 0; i < boneCount; ++i)
	//{
	//	size_t hash = std::hash<const char*>()(scene->mMeshes[0]->mBones[i]->mName.C_Str());
	//	boneIDMap[hash] = i;


	//	aiBone* bone = scene->mMeshes[0]->mBones[i];
	//	size_t aiNodeHash = std::hash<const char*>()(rootNode->FindNode(bone->mName)->mParent->mName.C_Str());

	//	Joint joint = Joint(bone->mOffsetMatrix.Inverse(), bone->mName.C_Str(), boneIDMap[aiNodeHash]);
	//	mySkeleton->myJoints.push_back(joint);
	//}





	//mySkeleton->myJoints.push_back(Joint(rootNode->mTransformation, 0));
	//for (unsigned int i = 1; i < boneCount; ++i)
	//{	
	//	mySkeleton->myJoints.push_back(Joint(
	//		scene->mMeshes[0]->mBones[i]->mOffsetMatrix
	//	, scene->mMeshes[0]->mBones[i]->mName
	//	, ))
	//
	//	mySkeleton->myJoints.push_back(Joint(rootNode->mChildren[i], i));
	//}

	/*mySkeletonPose = std::make_unique<SkeletonPose>();
	mySkeletonPose->mySkeleton = mySkeleton.get();
	mySkeletonPose->myLocalJointPoses.reserve(boneCount);

	for (unsigned int i = 0; i < boneCount; ++i)
	{
		mySkeletonPose->myLocalJointPoses[i].myRotKey = scene->mAnimations[0]->mChannels[0]->mRotationKeys[i];
		mySkeletonPose->myLocalJointPoses[i].myRotKey = scene->mAnimations[0]->mChannels[0]->mRotationKeys[i];
		mySkeletonPose->myLocalJointPoses[i].myRotKey = scene->mAnimations[0]->mChannels[0]->mRotationKeys[i];
	}


	size_t lastSlashIndex = aSkeletonFBXPath.find_last_of('/');
	std::string animFolder = aSkeletonFBXPath.substr(0, lastSlashIndex) + "/Animations/";

	for (const auto& file : std::filesystem::directory_iterator(animFolder))
	{
		if (file.path().has_extension())
		{
			if (file.path().extension() == ".fbx")
			{
				const std::string animationClipPath = file.path().string();
				const aiScene* animationScene = importer.ReadFile(animationClipPath.c_str(), aiProcessPreset_TargetRealtime_Quality | aiProcess_ConvertToLeftHanded);
				animationScene;


			}
		}

	}*/




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
		myScenes.emplace_back(importer.GetOrphanedScene());
		SaveMotionToMap(myScenes.back(), anFBXFilePath);

		//aiScene* scene = importer.GetOrphanedScene();
		//scene = myScenes.emplace_back(importer.GetOrphanedScene());
		//mySkeleton = std::make_unique<Skeleton>();
		//mySkeleton->jointCount = scene->mRootNode->mNumChildren;
		//auto jointCount = scene->mRootNode->mNumChildren;
		//mySkeleton->myJoints.resize(jointCount);
		//mySkeleton->myJoints[0].myInverseBindPose = scene->mRootNode->mTransformation;
		//mySkeleton->myJoints[0].myName = scene->mRootNode->mName.C_Str();
		//mySkeleton->myJoints[0].myParentIndex = 0;
		//for (unsigned int i = 0; i < jointCount; ++i)
		//{
		//	mySkeleton->myJoints[i].myInverseBindPose = scene->mRootNode->mChildren[i]->mTransformation;
		//	mySkeleton->myJoints[i].myName = scene->mRootNode->mChildren[i]->mName.C_Str();
		//	mySkeleton->myJoints[i].myParentIndex = i;
		//}
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
		aiMatrix4x4 matrix = aMesh->mBones[i]->mOffsetMatrix;
		matrix.Decompose(
			  myBoneInfo[BoneIndex].myPosition
			, myBoneInfo[BoneIndex].myRotation
			, myBoneInfo[BoneIndex].myScale);
		myBoneInfo[BoneIndex].myBoneOffset = matrix;

		for (uint j = 0; j < aMesh->mBones[i]->mNumWeights; j++)// Mass for physics?
		{
			uint VertexID = myEntries[aMeshIndex].myBaseVertex + aMesh->mBones[i]->mWeights[j].mVertexId;
			float Weight = aMesh->mBones[i]->mWeights[j].mWeight;
			myMass[VertexID].AddBoneData(BoneIndex, Weight);
		}
	}
}


void CAnimationController::SetBoneTransforms(int animIndex0, std::array<aiMatrix4x4, 64>& outTransforms)
{
	unsigned int jointCount = CountChildren(myScenes[animIndex0]->mRootNode);
	std::vector<aiMatrix4x4> jointPoses0;
	jointPoses0.reserve(jointCount);
	ReadNodeHeirarchy(
		myScenes[animIndex0]
		, myTicks0
		, myScenes[animIndex0]->mRootNode
		, jointPoses0);

	std::vector<aiMatrix4x4> jointPoses1;
	jointPoses1.reserve(jointCount);
	ReadNodeHeirarchy(
		  myScenes[myAnimIndex1]
		, myTicks1
		, myScenes[myAnimIndex1]->mRootNode	
		, jointPoses1);

	std::vector<aiMatrix4x4> nodeTransformations;
	for (uint i = 0; i < jointPoses0.size(); i++)
	{
		nodeTransformations.push_back(BlendMatrix(jointPoses0[i], jointPoses1[i], myBlendingTime));	
	}

	auto iterator = nodeTransformations.begin();
	CalculateWorldTransforms(outTransforms, iterator, myScenes[myAnimIndex0]->mRootNode, myGlobalInverseTransform);
}

void CAnimationController::ReadNodeHeirarchy(const aiScene* aScene, float tick, const aiNode* aNode, std::vector<aiMatrix4x4>& outNodeTransformations)
{
	aiMatrix4x4 nodeTransformation(aNode->mTransformation);
	const aiNodeAnim* nodeAnimation = FindNodeAnim(aScene->mAnimations[0], aNode->mName.data);
	
	if (nodeAnimation)
	{
		aiMatrix4x4 ScalingM;
		aiVector3D scale = CalculateInterpolation(nodeAnimation->mScalingKeys, nodeAnimation->mNumScalingKeys, tick).mValue;
		aiMatrix4x4::Scaling(scale, ScalingM);

		aiMatrix4x4 RotationM;
		aiQuaternion rot = CalculateInterpolation(nodeAnimation->mRotationKeys, nodeAnimation->mNumRotationKeys, tick).mValue;
		InitM4FromM3(RotationM, rot.GetMatrix());

		aiMatrix4x4 TranslationM;
		aiVector3D pos = CalculateInterpolation(nodeAnimation->mPositionKeys, nodeAnimation->mNumPositionKeys, tick).mValue;
		aiMatrix4x4::Translation(pos, TranslationM);

		nodeTransformation = TranslationM * RotationM * ScalingM;
	}
	outNodeTransformations.push_back(nodeTransformation);

	for (uint i = 0; i < aNode->mNumChildren; i++)
		ReadNodeHeirarchy(aScene, tick, aNode->mChildren[i], outNodeTransformations);
}

void CAnimationController::CalculateWorldTransforms(std::array<aiMatrix4x4, 64>& outTransforms, std::vector<aiMatrix4x4>::const_iterator& aNodeTransformationIterator, const aiNode* aNode, const aiMatrix4x4& aParentTransform)
{
	
	aiMatrix4x4 globalTransformation = aParentTransform * (*aNodeTransformationIterator);
	if (myBoneMapping.find(aNode->mName.data) != myBoneMapping.end())
	{
		uint boneIndex = myBoneMapping[aNode->mName.data];
		outTransforms[boneIndex] =  globalTransformation * myBoneInfo[boneIndex].myBoneOffset;
	}

	for (unsigned int i = 0; i < aNode->mNumChildren; ++i)
		CalculateWorldTransforms(outTransforms, ++aNodeTransformationIterator, aNode->mChildren[i], globalTransformation);
}

void CAnimationController::UpdateAnimationTimes(std::array<SlimMatrix44, 64>& someBones)
{
	float dt = CTimer::Dt();

	myTicks0 = fmodf(myTicks0 + dt, AnimationDurationInSeconds(myAnimIndex0));//duration0 /*- dt*/);
	myTicks1 = fmodf(myTicks1 + dt, AnimationDurationInSeconds(myAnimIndex1));

	std::array<aiMatrix4x4, 64> trans;
	SetBoneTransforms(myAnimIndex0, trans);
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

aiMatrix4x4 CAnimationController::BlendMatrix(const aiMatrix4x4& aMatrixA, const aiMatrix4x4& aMatrixB, float aBlend) const
{
	aiVector3D scaleStart, scaleEnd, scaleResult;
	aiQuaternion rotationStart, rotationEnd, rotationResult;
	aiVector3D posStart, posEnd, posResult;
	aMatrixA.Decompose(scaleStart, rotationStart, posStart);
	aMatrixB.Decompose(scaleEnd, rotationEnd, posEnd);

	Assimp::Interpolator<aiVector3D>()(posResult, posStart, posEnd, aBlend);
	aiQuaternion::Interpolate(rotationResult, rotationStart, rotationEnd, aBlend);
	Assimp::Interpolator<aiVector3D>()(scaleResult, scaleStart, scaleEnd, aBlend);
	rotationResult.Normalize();

	aiMatrix4x4 scaleM;
	aiMatrix4x4::Scaling(scaleResult, scaleM);

	aiMatrix4x4 rotM;
	InitM4FromM3(rotM, rotationResult.GetMatrix());

	aiMatrix4x4 TranslationM;
	aiMatrix4x4::Translation(posResult, TranslationM);

	aiMatrix4x4 transformation = TranslationM * rotM * scaleM;
	return transformation;
}

unsigned int CAnimationController::CountChildren(const aiNode* aNode)
{
	unsigned int nodeCount = 0;
	for (uint i = 0; i < aNode->mNumChildren; i++)
	{
		nodeCount += CountChildren(aNode->mChildren[i]);
	}
	return nodeCount;
}