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

void CAnimationController::SetBoneTransforms(int animIndex0, int animIndex1, float aBlend, std::array<aiMatrix4x4, 64>& outTransforms)
{
	std::vector<BoneInfoAnim> jointPoses0;
	{
		aiMatrix4x4 identity;
		InitIdentityM4(identity);
		jointPoses0.resize(myBoneInfo.size());
		ReadNodeHeirarchy(
			myScenes[animIndex0]
			, myTicks0
			, myScenes[animIndex0]->mRootNode
			, identity
			, jointPoses0);
	}
	std::vector<BoneInfoAnim> jointPoses1;
	{
		aiMatrix4x4 identity;
		InitIdentityM4(identity);
		jointPoses1.resize(myBoneInfo.size());
		ReadNodeHeirarchy(
			  myScenes[animIndex1]
			, myTicks1
			, myScenes[animIndex1]->mRootNode
			, identity
			, jointPoses1);
	}
	std::vector<BoneInfoAnim> blendedPoses;
	blendedPoses.resize(jointPoses0.size());
	for (int i = 0; i < jointPoses0.size(); ++i)
	{
		aiVector3D pos0		= jointPoses0[i].myPosition;
		aiQuaternion rot0	= jointPoses0[i].myRotation;
		aiVector3D scale0	= jointPoses0[i].myScale;

		aiVector3D pos1		= jointPoses1[i].myPosition;
		aiQuaternion rot1	= jointPoses1[i].myRotation;
		aiVector3D scale1	= jointPoses1[i].myScale;
	
		Assimp::Interpolator<aiVector3D>()(blendedPoses[i].myScale, scale0, scale1, aBlend);
		Assimp::Interpolator<aiQuaternion>()(blendedPoses[i].myRotation, rot0, rot1, aBlend);
		Assimp::Interpolator<aiVector3D>()(blendedPoses[i].myPosition, pos0, pos1, aBlend);
	}

	for (uint i = 0; i < myNumOfBones; i++)
	{
		aiMatrix4x4 scalingM = {};
		aiMatrix4x4::Scaling(blendedPoses[i].myScale, scalingM);
		
		aiMatrix4x4 translationM = {};
		aiMatrix4x4::Translation(blendedPoses[i].myPosition, translationM);
		
		aiMatrix4x4 rotationM;
		InitM4FromM3(rotationM, blendedPoses[i].myRotation.GetMatrix());

	}
}

void CAnimationController::SetBoneTransforms(int animIndex0, std::array<aiMatrix4x4, 64>& outTransforms)
{
	aiMatrix4x4 identity0;
	InitIdentityM4(identity0);
	std::vector<BoneInfoAnim> jointPoses0;
	jointPoses0.resize(myBoneInfo.size());
	ReadNodeHeirarchy(
		myScenes[animIndex0]
		, myTicks0
		, myScenes[animIndex0]->mRootNode
		, identity0
		, jointPoses0);

	aiMatrix4x4 identity1;
	InitIdentityM4(identity1);
	std::vector<BoneInfoAnim> jointPoses1;
	jointPoses1.resize(myBoneInfo.size());
	ReadNodeHeirarchy(
		  myScenes[myAnimIndex1]
		, myTicks1
		, myScenes[myAnimIndex1]->mRootNode	
		, identity1
		, jointPoses1);

	for (uint i = 0; i < myNumOfBones; i++)
	{
		aiMatrix4x4 offsetA = jointPoses0[i].myBoneOffset;
		aiMatrix4x4 offsetB = jointPoses1[i].myBoneOffset;
		aiMatrix4x4 offset = BlendMatrix(offsetA, offsetB, myBlendingTime);

		aiMatrix4x4 parentA = jointPoses0[i].myParentTransform;
		aiMatrix4x4 parentB = jointPoses1[i].myParentTransform;
		aiMatrix4x4 parent = BlendMatrix(parentA, parentB, myBlendingTime);

		aiMatrix4x4 nodeTransformationA = jointPoses0[i].myNodeTransformation;
		aiMatrix4x4 nodeTransformationB = jointPoses1[i].myNodeTransformation;
		aiMatrix4x4 nodeTransformation = parent * BlendMatrix(nodeTransformationA, nodeTransformationB, myBlendingTime);

		outTransforms[i] = nodeTransformation; 
		/*myGlobalInverseTransform **/  /** offset*/;
		//aiMatrix4x4 parent = BlendMatrix(parentA, parentB, myBlendingTime);
		/*parent * */
			////myGlobalInverseTransform * nodeTransformation * offset; ///jointPoses0[i].myFinalTransformation;
	}

	for (uint i = 0; i < myNumOfBones; ++i)
	{
		outTransforms[i] = myGlobalInverseTransform * outTransforms[i] * myBoneInfo[i].myBoneOffset;
	}

	//CalculateWorldTransforms(outTransforms, myScenes[animIndex0]->mRootNode);
}


/*
		aiVector3D scaleStart, scaleEnd, scaleResult;
		aiQuaternion rotationStart, rotationEnd, rotationResult;
		aiVector3D posStart, posEnd, posResult;
		matrixA.Decompose(scaleStart, rotationStart, posStart);
		matrixB.Decompose(scaleEnd, rotationEnd, posEnd);

		Assimp::Interpolator<aiVector3D>()(posResult, posStart, posEnd, myBlendingTime);
		aiQuaternion::Interpolate(rotationResult, rotationStart, rotationEnd, myBlendingTime);
		Assimp::Interpolator<aiVector3D>()(scaleResult, scaleStart, scaleEnd, myBlendingTime);
		rotationResult.Normalize();

		aiMatrix4x4 scaleM;
		aiMatrix4x4::Scaling(scaleResult, scaleM);

		aiMatrix4x4 rotM;
		InitM4FromM3(rotM, rotationResult.GetMatrix());

		aiMatrix4x4 TranslationM;
		aiMatrix4x4::Translation(posResult, TranslationM);

		aiMatrix4x4 nodeTransformation = TranslationM * rotM * scaleM;

		aiMatrix4x4 offsetA = jointPoses0[i].myBoneOffset;
		aiMatrix4x4 offsetB = jointPoses1[i].myBoneOffset;

		aiVector3D offsetScaleA, offsetScaleB, offsetScale;
		aiQuaternion offsetRotationA, offsetRotationB, offsetRotation;
		aiVector3D offsetPositionA, offsetPositionB, offsetPosition;

		offsetA.Decompose(offsetScaleA, offsetRotationA, offsetPositionA);
		offsetB.Decompose(offsetScaleB, offsetRotationB, offsetPositionB);

		Assimp::Interpolator<aiVector3D>()(offsetScale, offsetScaleA, offsetScaleB, myBlendingTime);
		aiQuaternion::Interpolate(offsetRotation, offsetRotationA, offsetRotationB, myBlendingTime);
		Assimp::Interpolator<aiVector3D>()(offsetPosition, offsetPositionA, offsetPositionB, myBlendingTime);
		offsetRotation.Normalize();

		aiMatrix4x4 offsetScaleM;
		aiMatrix4x4::Scaling(offsetScale, offsetScaleM);

		aiMatrix4x4 offsetRotationM;
		InitM4FromM3(offsetRotationM, offsetRotation.GetMatrix());

		aiMatrix4x4 offsetTranslationM;
		aiMatrix4x4::Translation(posResult, offsetTranslationM);

		aiMatrix4x4 offsetMatrix = offsetTranslationM * offsetRotationM * offsetScaleM;
		

		outTransforms[i] = myGlobalInverseTransform * nodeTransformation * offsetMatrix;*///jointPoses0[i].myFinalTransformation;
	/*	aiMatrix4x4 GlobalTransformation = jointPoses0[i].myParentTransform * TranslationM * rotM * scaleM ;
		jointPoses0[i].myFinalTransformation = myGlobalInverseTransform * GlobalTransformation * jointPoses0[i].myBoneOffset;*/
	

	//for(int i = 0; i < jointPoses0.size(); ++i)
	//	outTransforms[i] = jointPoses0[i].myFinalTransformation;


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
		//ReadNodeHeirarchy(
		//	  myScenes[myAnimIndex0]
		//	, myTicks0
		//	, myScenes[myAnimIndex0]->mRootNode
		//	, Identity);

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
	  const aiScene* aScene
	, float tick
	, const aiNode* aNode
	//, const aiMatrix4x4& aParentTransform
	, std::vector<BoneInfoAnim>& outJointPoses)
{
	aiMatrix4x4 NodeTransformation(aNode->mTransformation);
	const aiNodeAnim* nodeAnimation = FindNodeAnim(aScene->mAnimations[0], aNode->mName.data);
	aiVector3D pos;
	aiQuaternion rot;
	aiVector3D scale;

	if (nodeAnimation)
	{
		aiMatrix4x4 ScalingM;
		scale = CalculateInterpolation(nodeAnimation->mScalingKeys, nodeAnimation->mNumScalingKeys, tick).mValue;
		aiMatrix4x4::Scaling(scale, ScalingM);

		aiMatrix4x4 RotationM;
		rot = CalculateInterpolation(nodeAnimation->mRotationKeys, nodeAnimation->mNumRotationKeys, tick).mValue;
		InitM4FromM3(RotationM, rot.GetMatrix());

		aiMatrix4x4 TranslationM;
		pos = CalculateInterpolation(nodeAnimation->mPositionKeys, nodeAnimation->mNumPositionKeys, tick).mValue;
		aiMatrix4x4::Translation(pos, TranslationM);

		// Combine the above transformations	
		NodeTransformation = TranslationM * RotationM * ScalingM;
	}

	//aiMatrix4x4 GlobalTransformation = aParentTransform * NodeTransformation;
	if (myBoneMapping.find(aNode->mName.data) != myBoneMapping.end())
	{
		uint BoneIndex = myBoneMapping[aNode->mName.data];
		outJointPoses[BoneIndex].myNodeTransformation = NodeTransformation;
		outJointPoses[BoneIndex].myBoneOffset = myBoneInfo[BoneIndex].myBoneOffset;
		outJointPoses[BoneIndex].myParentTransform = aNode->mParent->mTransformation;
	}

	for (uint i = 0; i < aNode->mNumChildren; i++)
		ReadNodeHeirarchy(aScene, tick, aNode->mChildren[i], outJointPoses);
}

void CAnimationController::ReadNodeHeirarchy(const aiScene* aScene, float tick, const aiNode* aNode, const aiMatrix4x4& aParentTransform, std::vector<BoneInfoAnim>& outJointPoses)
{
	aiMatrix4x4 NodeTransformation(aNode->mTransformation);
	const aiNodeAnim* nodeAnimation = FindNodeAnim(aScene->mAnimations[0], aNode->mName.data);
	aiVector3D pos;
	aiQuaternion rot;
	aiVector3D scale;

	if (nodeAnimation)
	{
		aiMatrix4x4 ScalingM;
		scale = CalculateInterpolation(nodeAnimation->mScalingKeys, nodeAnimation->mNumScalingKeys, tick).mValue;
		aiMatrix4x4::Scaling(scale, ScalingM);

		aiMatrix4x4 RotationM;
		rot = CalculateInterpolation(nodeAnimation->mRotationKeys, nodeAnimation->mNumRotationKeys, tick).mValue;
		InitM4FromM3(RotationM, rot.GetMatrix());

		aiMatrix4x4 TranslationM;
		pos = CalculateInterpolation(nodeAnimation->mPositionKeys, nodeAnimation->mNumPositionKeys, tick).mValue;
		aiMatrix4x4::Translation(pos, TranslationM);

		// Combine the above transformations	
		NodeTransformation = TranslationM * RotationM * ScalingM;
	}

	aiMatrix4x4 GlobalTransformation = aParentTransform * NodeTransformation;
	if (myBoneMapping.find(aNode->mName.data) != myBoneMapping.end())
	{
		uint BoneIndex = myBoneMapping[aNode->mName.data];
		outJointPoses[BoneIndex].myBoneOffset = myBoneInfo[BoneIndex].myBoneOffset;
		outJointPoses[BoneIndex].myNodeTransformation = NodeTransformation;
		outJointPoses[BoneIndex].myParentTransform = aParentTransform;
	}

	for (uint i = 0; i < aNode->mNumChildren; i++)
		ReadNodeHeirarchy(aScene, tick, aNode->mChildren[i], GlobalTransformation, outJointPoses);
}

void CAnimationController::CalculateWorldTransforms(std::array<aiMatrix4x4, 64>& outTransforms, const aiNode* aNode)
{
	//for (unsigned int i = 0; i < aNode->mNumChildren; ++i)
	//	CalculateWorldTransforms(outTransforms, aNode->mChildren[i]);

	//if (aNode->mParent != nullptr)
	//{
	//	uint boneIndex = myBoneMapping[aNode->mName.data];
	//	aiMatrix4x4 GlobalTransformation = aNode->mParent->mTransformation * outTransforms[boneIndex];
	//	outTransforms[boneIndex] = myGlobalInverseTransform * GlobalTransformation * myBoneInfo[boneIndex].myBoneOffset;
	//	//outTransforms[boneIndex] = myGlobalInverseTransform * aNode->mParent->mTransformation * outTransforms[boneIndex] * myBoneInfo[boneIndex].myBoneOffset;
	//}

	//if (myBoneMapping.find(aNode->mName.data) != myBoneMapping.end())
	//{
	//	uint boneIndex = myBoneMapping[aNode->mName.data];
	//	aiMatrix4x4 GlobalTransformation = aNode->mParent->mTransformation * outTransforms[boneIndex];
	//	outTransforms[boneIndex] = myGlobalInverseTransform * GlobalTransformation * myBoneInfo[boneIndex].myBoneOffset;
	//}

	//for (unsigned int i = 1; i < 64; ++i)
	//{
	//	aiMatrix4x4 GlobalTransformation = aNode->mParent->mTransformation * outTransforms[i];
	//	outTransforms[i] = myGlobalInverseTransform * GlobalTransformation * myBoneInfo[i].myBoneOffset;
	//}

	//if (myBoneMapping.find(aNode->mName.data) != myBoneMapping.end())
	//{
	//	uint boneIndex = myBoneMapping[aNode->mName.data];
	//	aiMatrix4x4 GlobalTransformation = aNode->mParent->mTransformation * outTransforms[boneIndex];
	//	outTransforms[boneIndex] = myGlobalInverseTransform * GlobalTransformation * myBoneInfo[boneIndex].myBoneOffset;
	//		//myGlobalInverseTransform * outTransforms[boneIndex] * aNode->mParent->mTransformation * myBoneInfo[boneIndex].myBoneOffset; /** myBoneInfo[boneIndex].myBoneOffset*/
	//	//std::cout << aNode->mName.C_Str() << " --> " << aNode->mParent->mName.C_Str() << std::endl;
	//}

	//for(unsigned int i = 0; i < aNode->mNumChildren; ++i)
	//	CalculateWorldTransforms(outTransforms, aNode->mChildren[i]);
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
		//Add the lerped-positional data here as well! And save it to a "outBoneInfo" instead of myBoneInfo!!!
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

	size_t size = someBlendingValues.size();
	aiMatrix4x4 NodeTransformation0 = aNode->mTransformation;
	
	const aiNodeAnim* anim0 = FindNodeAnim(someScenes[0]->mAnimations[0], aNode->mName.data);
	if (anim0 != nullptr)
	{	
		aiVectorKey scaleKeyResult = CalculateInterpolation(anim0->mScalingKeys, anim0->mNumScalingKeys, someTicks[0]);
		aiQuatKey rotationKeyResult = CalculateInterpolation(anim0->mRotationKeys, anim0->mNumRotationKeys, someTicks[0]);
		aiVectorKey positionKeyResult = CalculateInterpolation(anim0->mPositionKeys, anim0->mNumPositionKeys, someTicks[0]);

		//size_t one = 1;
		//size_t size = someBlendingValues.size();
		for (int i = 1; i < size; ++i)
		{
			//aiMatrix4x4 nodeTransform(someNodes[i - one]->mTransformation);
			const aiNodeAnim* anim = FindNodeAnim(someScenes[i]->mAnimations[0], aNode->mName.data);
			if (anim == nullptr)
				continue;

			//aiVectorKey scaleKey = CalculateInterpolation(anim->mScalingKeys, anim->mNumScalingKeys, someTicks[i - one]);
			aiVectorKey nextScaleKey = CalculateInterpolation(anim->mScalingKeys, anim->mNumScalingKeys, someTicks[i]);
			aiVector3D scale = {};
			Assimp::Interpolator<aiVectorKey>()(
				scale
				, scaleKeyResult
				, nextScaleKey
				, someBlendingValues[i]);
			aiMatrix4x4 scaleM = {};
			scaleKeyResult.mValue = scale;
			//aiMatrix4x4::Scaling(scale, scaleM);


			//aiQuatKey rotationKey = CalculateInterpolation(anim->mRotationKeys, anim->mNumRotationKeys, someTicks[i - one]);
			aiQuatKey nextRotationKey = CalculateInterpolation(anim->mRotationKeys, anim->mNumRotationKeys, someTicks[i]);
			aiQuaternion rotation = {};
			Assimp::Interpolator<aiQuatKey>()(
				rotation
				, rotationKeyResult
				, nextRotationKey
				, someBlendingValues[i]);
			//aiMatrix4x4 rotationM = {};
			//InitM4FromM3(rotationM, rotation.GetMatrix());
			rotationKeyResult.mValue = rotation;

			//aiVectorKey positionKey = CalculateInterpolation(anim->mPositionKeys, anim->mNumPositionKeys, someTicks[i - one]);
			aiVectorKey nextPositionKey = CalculateInterpolation(anim->mPositionKeys, anim->mNumPositionKeys, someTicks[i]);
			aiVector3D position = {};
			Assimp::Interpolator<aiVectorKey>()(
				position
				, positionKeyResult
				, nextPositionKey
				, someBlendingValues[i]);
			//aiMatrix4x4 translationM = {};
			//aiMatrix4x4::Translation(position, translationM);
			positionKeyResult.mValue = position;
		}
		aiMatrix4x4 scaleM = {};
		aiMatrix4x4::Scaling(scaleKeyResult.mValue, scaleM);

		aiMatrix4x4 rotationM = {};
		InitM4FromM3(rotationM, rotationKeyResult.mValue.GetMatrix());

		aiMatrix4x4 translationM = {};
		aiMatrix4x4::Translation(positionKeyResult.mValue, translationM);

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



//void CAnimationController::CountChildren(const aiNode* aNode, int& count, int& childIndex)
//{
//	if (aNode->mNumChildren == 0)
//		return CountChildren(aNode->mParent, count, ++childIndex);
//
//	count += aNode->mNumChildren;
//	return CountChildren(aNode->mChildren[childIndex], count, childIndex);
//}
//	//if (aNode->mNumChildren < childIndex)
	//{
	//	return CountChildren(aNode->mParent, count, ++childIndex);
	//}
//	else
	//{
	//}


	//for (int i = aNode->mNumChildren; i != 0; --i)
	//{
	//	count += aNode->mNumChildren;
	//	return CountChildren(aNode->mChildren[i - 1], count);
	//}


//unsigned int CAnimationController::CountChildren(const aiNode* aNode)
//{
//	uint childCount = aNode->mNumChildren;
//	for (uint i = 0; i < childCount; i++)
//	{
//		return 1 + CountChildren(aNode->mChildren[i]);
//	}
//	
//	//return 1;
//}



	//for (unsigned int i = 0; i < aNode->mNumChildren; ++i)
	//{
	//	const aiNode* child = aNode->mChildren[i];
	//}


	//if (aNode->mNumChildren == 0)
	//{
	//	return 1;
	//}

//ReadNodeHeirarchy(aFromScene, aToScene, aTickFrom, aTickTo, aNodeFrom->mChildren[i], aNodeTo->mChildren[i], GlobalTransformation);

	//for(int i = 0; i < static_cast<int>(aNode->mNumChildren); ++i)
	//	return 1 + CountChildren(aNode->mChildren[0]);



	//return CountChildren(aNode->mChildren[) + aNode->

