#include "stdafx.h"
#include "AnimationController.h"
#include <fstream>
#include "assimp/Importer.hpp"	//OO version Header!
#include "assimp/postprocess.h"
#include "assimp/LogStream.hpp"
#include <AssImp\anim.h>
#include <math.h>
#include "EngineException.h"
#include <filesystem>


CAnimationController::CAnimationController()
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
//void CAnimationController::ImportSkeleton(const std::string& aSkeletonFBXPath)
//{
//	ENGINE_BOOL_POPUP(aSkeletonFBXPath.length(), "Imported Skeleton path was empty");
//
//	// Check if file exists
//	std::ifstream fIn(aSkeletonFBXPath.c_str());
//	ENGINE_BOOL_POPUP(!fIn.fail(), "Failed to read Imported Skeleton %s", aSkeletonFBXPath.c_str());
//	fIn.close();
//	Assimp::Importer importer = {};
//	importer.ReadFile(aSkeletonFBXPath.c_str(), aiProcessPreset_TargetRealtime_Quality | aiProcess_ConvertToLeftHanded);
//
//	//Next Step is to Follow the Book proper!
//	//Best idea is probably to make sure that i understand how the Import Really works It's at least more than 1 step
//		//1 -> Load Assimp::aiScene*
//		//2 -> Init Bones 
//		//3 -> seems like it's here the vertex-id's are corrolated to each bone!
//		//Maybe i don't need to refactor the import things!
//		//Only reason to do so is if the book-implementation requires it! : )
//			 //Remember -> The book can bring me all the way to Inverse Kinematics! Hopefully...!
//}
bool CAnimationController::ImportRig(const std::string& anFBXFilePath/*, const std::vector<std::string>& someAnimationPaths*/)
{
	assert(anFBXFilePath.length() > 0 && "FBX Rig FileName is Empty.");
	assert(std::filesystem::exists(anFBXFilePath) && "FBX Rig File Does not Exist.");

	Assimp::Importer importer;
	if (!importer.ReadFile(anFBXFilePath, aiProcessPreset_TargetRealtime_Quality | aiProcess_ConvertToLeftHanded))
		assert(false && "Failed to Read FBX Rig File.");

	myScenes.emplace_back(importer.GetOrphanedScene());
	SaveMotionToMap(myScenes.back(), anFBXFilePath);

	myGlobalInverseTransform = myScenes[0]->mRootNode->mTransformation;
	myGlobalInverseTransform.Inverse();

	for (uint i = 0; i < myScenes[0]->mNumMeshes; ++i)
		LoadBones(i, myScenes[0]->mMeshes[i]);
	
	return true;
}

//void CAnimationController::ReadCreateSkeleton(const aiNode* aNode, Skeleton& outSkeleton)
//{
//
//
//	//aiMatrix4x4 inverseBindPose = {};
//	//outSkeleton.myJoints.push_back({ aNode-> })
//
//	Joint joint = {};
//	//joint.myName = aNode->mName.C_Str();
//	//joint.myParentIndex = aNode->mParent->mName;
//	//outSkeleton.myJoints.push_back()
//
//	for (uint i = 0; i < aNode->mNumChildren; ++i)
//		ReadCreateSkeleton(aNode->mChildren[i], outSkeleton);
//}

bool CAnimationController::ImportAnimation(const std::string& anFBXFilePath)
{
	assert(anFBXFilePath.length() > 0 && "FBX Animation FileName is Empty.");
	assert(std::filesystem::exists(anFBXFilePath) && "FBX Animation File Does not Exist.");

	Assimp::Importer importer;
	if (!importer.ReadFile(anFBXFilePath, aiProcessPreset_TargetRealtime_Quality | aiProcess_ConvertToLeftHanded))
		assert(false && "Failed to Read FBX Animation File.");

	myScenes.push_back(importer.GetOrphanedScene());
	SaveMotionToMap(myScenes.back(), anFBXFilePath);
	myMotionPaths.push_back(anFBXFilePath);

	// If the import failed, report it
	if (!myScenes.back())
		return false;

	assert(myScenes.back()->mNumAnimations > 0 && "Animation Clip has no Animations");
	return true;
}
bool CAnimationController::InitFromScene(const aiScene* pScene)
{


	return true;
}
void CAnimationController::LoadBones(uint aMeshIndex, const aiMesh* aMesh)
{
	uint numberOfBones = 0;
	for (uint i = 0; i < aMesh->mNumBones; i++)
	{
		uint BoneIndex = 0;
		std::string BoneName(aMesh->mBones[i]->mName.data);

		if (myBoneMapping.find(BoneName) == myBoneMapping.end())
		{
			BoneIndex = numberOfBones;
			numberOfBones++;
			myBoneInfo.push_back(Joint());
		}
		else
		{
			BoneIndex = myBoneMapping[BoneName];
		}

		myBoneMapping[BoneName] = BoneIndex;
		myBoneInfo[BoneIndex].myInverseBindPose = aMesh->mBones[i]->mOffsetMatrix;
		myBoneInfo[BoneIndex].myName = aMesh->mBones[i]->mName.C_Str();
	}
}

void CAnimationController::AnimateMotion(const int anIndex, const float aTick, std::array<aiMatrix4x4, 64>& outTransforms)
{
	unsigned int jointCount = 0;
	jointCount = CountJoints(myScenes[anIndex]->mRootNode);
	std::vector<JointPose> poses;
	poses.reserve(jointCount);
	ReadNodeHeirarchy(
		myScenes[anIndex]
		, aTick
		, myScenes[anIndex]->mRootNode
		, poses);

	auto iterator = poses.begin();
	CalculateWorldTransforms(outTransforms, iterator, myScenes[anIndex]->mRootNode, myGlobalInverseTransform);
}

void CAnimationController::AnimateMotionBlend1D(const int someIndexes[2], const float someTicks[2], const float blend, std::array<aiMatrix4x4, 64>& outTransforms)
{
	if (someIndexes[0] == someIndexes[1])
		return AnimateMotion(someIndexes[0], someTicks[0], outTransforms);

	unsigned int jointCount = 0;
	jointCount = CountJoints(myScenes[someIndexes[0]]->mRootNode);
	std::vector<JointPose> poses0;
	poses0.reserve(jointCount);
	ReadNodeHeirarchy(
		myScenes[someIndexes[0]]
		, someTicks[0]
		, myScenes[someIndexes[0]]->mRootNode
		, poses0);

	std::vector<JointPose> poses1;
	poses1.reserve(jointCount);
	ReadNodeHeirarchy(
		myScenes[someIndexes[1]]
		, someTicks[1]
		, myScenes[someIndexes[1]]->mRootNode
		, poses1);

	std::vector<JointPose> nodeTransformations;
	for (size_t i = 0; i < poses0.size(); i++)
	{
		JointPose finalPose = {};
		finalPose.aNodeTransformation = BlendMatrix(poses0[i].aNodeTransformation, poses1[i].aNodeTransformation, blend);
		nodeTransformations.push_back(finalPose);
	}

	auto iterator = nodeTransformations.begin();
	CalculateWorldTransforms(outTransforms, iterator, myScenes[someIndexes[0]]->mRootNode, myGlobalInverseTransform);
}

void CAnimationController::AnimateMotionBlend2D(const int someIndexes[3], const float someTicks[3], const float someBlends[3], std::array<aiMatrix4x4, 64>& outTransforms)
{
	unsigned int jointCount = 0;
	jointCount = CountJoints(myScenes[someIndexes[0]]->mRootNode);
	std::vector<JointPose> poses0;
	poses0.reserve(jointCount);
	ReadNodeHeirarchy(
		myScenes[someIndexes[0]]
		, someTicks[0]
		, myScenes[someIndexes[0]]->mRootNode
		, poses0);

	std::vector<JointPose> poses1;
	poses1.reserve(jointCount);
	ReadNodeHeirarchy(
		myScenes[someIndexes[1]]
		, someTicks[1]
		, myScenes[someIndexes[1]]->mRootNode
		, poses1);

	std::vector<JointPose> poses2;
	poses2.reserve(jointCount);
	ReadNodeHeirarchy(
		myScenes[someIndexes[2]]
		, someTicks[2]
		, myScenes[someIndexes[2]]->mRootNode
		, poses2);

	assert(poses0.size() == poses1.size() && poses1.size() == poses2.size());

	std::vector<JointPose> nodeTransformations;
	nodeTransformations.reserve(jointCount);
	for (size_t i = 0; i < poses0.size(); i++)
	{
		JointPose finalPose = {};
		aiMatrix4x4 first = BlendMatrix(poses0[i].aNodeTransformation, poses1[i].aNodeTransformation, someBlends[0]);
		aiMatrix4x4 second = BlendMatrix(poses0[i].aNodeTransformation, poses2[i].aNodeTransformation, someBlends[1]);
		finalPose.aNodeTransformation = BlendMatrix(first, second, someBlends[2]);

		nodeTransformations.push_back(finalPose);
	}
	auto iterator = nodeTransformations.begin();
	CalculateWorldTransforms(outTransforms, iterator, myScenes[someIndexes[0]]->mRootNode, myGlobalInverseTransform);
}

void CAnimationController::AnimateMotionBlendCorners(const int someIndexes[4], const float someTicks[4], const float someBlendValues[2], std::array<aiMatrix4x4, 64>& outTransforms)
{
	uint jointCount = CountJoints(myScenes[someIndexes[0]]->mRootNode);

	std::vector<JointPose> poses0;
	poses0.reserve(jointCount);
	CalculateNodePoses(myScenes[someIndexes[0]], myScenes[someIndexes[0]]->mRootNode, someTicks[0], poses0);

	std::vector<JointPose> poses1;
	poses1.reserve(jointCount);
	CalculateNodePoses(myScenes[someIndexes[1]], myScenes[someIndexes[1]]->mRootNode, someTicks[1], poses1);

	std::vector<JointPose> poses2;
	poses2.reserve(jointCount);
	CalculateNodePoses(myScenes[someIndexes[2]], myScenes[someIndexes[2]]->mRootNode, someTicks[2], poses2);

	std::vector<JointPose> poses3;
	poses3.reserve(jointCount);
	CalculateNodePoses(myScenes[someIndexes[3]], myScenes[someIndexes[3]]->mRootNode, someTicks[3], poses3);

	std::vector<JointPose> abTransformations;
	abTransformations.reserve(jointCount);
	for (size_t i = 0; i < poses0.size(); i++)
	{
		JointPose finalPose = {};
		finalPose.aNodeTransformation = BlendMatrix(poses0[i].aNodeTransformation, poses1[i].aNodeTransformation, someBlendValues[0]);
		abTransformations.push_back(finalPose);
	}

	std::vector<JointPose> cdTransformations;
	cdTransformations.reserve(jointCount);
	for (size_t i = 0; i < poses2.size(); i++)
	{
		JointPose finalPose = {};
		finalPose.aNodeTransformation = BlendMatrix(poses2[i].aNodeTransformation, poses3[i].aNodeTransformation, someBlendValues[0]);
		cdTransformations.push_back(finalPose);
	}

	std::vector<JointPose> transformations;
	transformations.reserve(jointCount);
	for (size_t i = 0; i < poses0.size(); ++i)
	{
		JointPose finalPose = {};
		finalPose.aNodeTransformation = BlendMatrix(abTransformations[i].aNodeTransformation, cdTransformations[i].aNodeTransformation, someBlendValues[1]);
		transformations.push_back(finalPose);
	}

	auto iterator = transformations.begin();
	CalculateWorldTransforms(outTransforms, iterator, myScenes[someIndexes[0]]->mRootNode, myGlobalInverseTransform);
}


void CAnimationController::ReadNodeHeirarchy(const aiScene* aScene, float tick, const aiNode* aNode, std::vector<JointPose>& outPoses)
{
	aiMatrix4x4 nodeTransformation(aNode->mTransformation);

	JointPose nodeJoint = {};
	nodeJoint.aNodeTransformation = aNode->mTransformation;

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

		nodeJoint.aNodeTransformation = TranslationM * RotationM * ScalingM;
		nodeJoint.myPosKey = pos;
		nodeJoint.myRotKey = rot;
		nodeJoint.myScaleKey = scale;
	}

	outPoses.push_back(nodeJoint);

	for (uint i = 0; i < aNode->mNumChildren; i++)
		ReadNodeHeirarchy(aScene, tick, aNode->mChildren[i], outPoses);
}


void CAnimationController::CalculateNodePoses(const aiScene* aScene, const aiNode* aNode, float aTick, std::vector<JointPose>& outJointPoses)
{
	unsigned int jointCount = 0;
	jointCount = CountJoints(aNode);
	std::vector<JointPose> poses;
	poses.reserve(jointCount);
	ReadNodeHeirarchy(
		aScene
		, aTick
		, aNode
		, outJointPoses);
}

void CAnimationController::CalculateWorldTransforms(std::array<aiMatrix4x4, 64>& outTransforms, std::vector<JointPose>::const_iterator& aNodeTransformationIterator, const aiNode* aNode, const aiMatrix4x4& aParentTransform)
{

	aiMatrix4x4 globalTransformation = aParentTransform * (*aNodeTransformationIterator).aNodeTransformation;
	if (myBoneMapping.find(aNode->mName.data) != myBoneMapping.end())
	{
		uint boneIndex = myBoneMapping[aNode->mName.data];
		outTransforms[boneIndex] = globalTransformation * myBoneInfo[boneIndex].myInverseBindPose;
	}

	for (unsigned int i = 0; i < aNode->mNumChildren; ++i)
		CalculateWorldTransforms(outTransforms, ++aNodeTransformationIterator, aNode->mChildren[i], globalTransformation);
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

unsigned int CAnimationController::CountJoints(const aiNode* aNode)
{
	unsigned int count = aNode->mNumChildren;
	for (uint i = 0; i < aNode->mNumChildren; i++)
	{
		count += CountJoints(aNode->mChildren[i]);
	}
	return count;
}

void CAnimationController::SaveMotionToMap(const aiScene* aScene, const std::string& aFilePath)
{
	auto lastSlash = aFilePath.find_last_of('/') + 1;
	std::string animName = aFilePath.substr(lastSlash, aFilePath.size() - lastSlash);
	mySceneMap[animName] = aScene;
	//myTicks.push_back(0.0f);
}

const int CAnimationController::MotionCount() const
{
	return static_cast<uint>(myScenes.size());
}
const float CAnimationController::MotionDurationInTicks(int anIndex) const
{
	return static_cast<float>(myScenes[anIndex]->mAnimations[0]->mDuration);
}
const float CAnimationController::MotionTPS(int anIndex) const
{
	return static_cast<float>(myScenes[anIndex]->mAnimations[0]->mTicksPerSecond);
}
std::vector<std::string> CAnimationController::GetMotionNames()
{
	std::vector<std::string> keys;
	for (auto& keyValue : mySceneMap)
		keys.push_back(keyValue.first);

	return keys;
}
const int CAnimationController::AnimationIndex(const std::string& aMotionName)
{
	const aiScene* scene = mySceneMap[aMotionName];
	for (int i = 0; i < myScenes.size(); ++i)
		if (scene == myScenes[i])
			return i;

	return 0;
}