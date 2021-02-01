#include "stdafx.h"
#include "AnimationController.h"

#include "../ModelLoader/modelExceptionTools.h"

CAnimationController::CAnimationController()
	: myNumOfBones(0)
	, myCurAnimIndex(0)
	, myPrevAnimIndex(0)
	, myRotation(0.f)
	, myBlendingTime(0.f)
	, myBlendingTimeMul(1.f)
	, myUpdateBoth(true)
	, myTemporary(false)
	, myPlayTime(0.f)
	, myAnimationTimeCurrent(0)
	, myAnimationTimePrev(0)
	, myAnimationTimePercent(0)
{}

CAnimationController::~CAnimationController()
{
	for (uint i = 0; i < myAnimationScenes.size(); ++i)
	{
		delete myAnimationScenes[i];
		myAnimationScenes[i] = nullptr;
	}
	myAnimationScenes.clear();
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

	myCurAnimIndex = static_cast<int>(myAnimationScenes.size());
	myAnimationScenes.emplace_back(new Assimp::Importer);

	if (ModelExceptionTools::IsDestructibleModel(anFBXFilePath))
	{
		int qualityFlags = aiProcessPreset_TargetRealtime_Quality;
		myAnimations.push_back(myAnimationScenes[myCurAnimIndex]->ReadFile(anFBXFilePath, (qualityFlags ^= (int)aiProcess_JoinIdenticalVertices) | aiProcess_ConvertToLeftHanded));
	}
	else
	{
		myAnimations.push_back(myAnimationScenes[myCurAnimIndex]->ReadFile(anFBXFilePath, aiProcessPreset_TargetRealtime_Quality | aiProcess_ConvertToLeftHanded));
	}
	//_curScene = importer.ReadFile( m_ModelPath, aiProcess_Triangulate | aiProcess_GenSmoothNormals );

	bool ret = false;
	// If the import failed, report it
	if (myAnimations[myCurAnimIndex])
	{
		myGlobalInverseTransform = myAnimations[myCurAnimIndex]->mRootNode->mTransformation;
		myGlobalInverseTransform.Inverse();
		ret = InitFromScene(myAnimations[myCurAnimIndex]);
		// Now we can access the file's contents.
		logInfo("Import of _curScene " + anFBXFilePath + " succeeded.");
	}
	else
	{
		logInfo(myAnimationScenes[myCurAnimIndex]->GetErrorString());
	}

	// We're done. Everything will be cleaned up by the importer destructor
	return ret;
}

bool CAnimationController::ImportAnimation(const std::string& fileName)
{
	// Check if file exists
	std::ifstream fin(fileName.c_str());
	if (!fin.fail())
	{
		fin.close();
	}
	else
	{
		MessageBoxA(NULL, ("Couldn't open file: " + fileName).c_str(), "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return false;
	}

	myCurAnimIndex = static_cast<int>(myAnimationScenes.size());
	myAnimationScenes.emplace_back(new Assimp::Importer);
	myAnimations.emplace_back(myAnimationScenes[myCurAnimIndex]->ReadFile(fileName, aiProcessPreset_TargetRealtime_Quality | aiProcess_ConvertToLeftHanded));

	// If the import failed, report it
	if (!myAnimations[myCurAnimIndex])
	{
		logInfo(myAnimationScenes[myCurAnimIndex]->GetErrorString());
		return false;
	}
	return true;
}

bool CAnimationController::InitFromScene(const aiScene* pScene)
{
	myAnimationTimePrev = 0.0f;
	myAnimationTimeCurrent = 0.0f;

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
	float time0(anAnimationTimeFrom);// Why is AnimationTime0 made into a local variable?
	float time1(anAnimationTimeTo);// Why is AnimationTime1 made into a local variable?

	std::string NodeName0(aStartNodeFrom->mName.data);// rename pNode0/1 is the previous animations data. pNode0 is pointerToPreviousAnimNode
	std::string NodeName1(aStartNodeTo->mName.data);// pointerToCurrentAnimNode
	
// Commented cause it was annoying when testing 2021 01 25
	//assert(NodeName0 == NodeName1);// their first node should be the same, roots must be equal

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

	uint n = min(aStartNodeFrom->mNumChildren, aStartNodeTo->mNumChildren); // Does one movement for all the children
	for (uint i = 0; i < n; i++)
	{
		ReadNodeHeirarchy(aFromScene, aToScene, anAnimationTimeFrom, anAnimationTimeTo, aStartNodeFrom->mChildren[i], aStartNodeTo->mChildren[i], GlobalTransformation, aStopAnimAtLevel);
	}
}

void CAnimationController::SetBoneTransforms(std::vector<aiMatrix4x4>& aTransformsVector)
{
	aiMatrix4x4 Identity;// Used for ReadNodeHierarchy
	InitIdentityM4(Identity);

	if (myBlendingTime > 0.f)// There are 2 animations that we are blending between
	{
	// Animation time for the first anim ( 0 ) (Previous?)
		// Ticks == Frames
		float TicksPerSecond = 
			static_cast<float>(myAnimations[myPrevAnimIndex]->mAnimations[0]->mTicksPerSecond) != 0 
			? 
			static_cast<float>(myAnimations[myPrevAnimIndex]->mAnimations[0]->mTicksPerSecond) : 25.0f;
		
		// How many frames are we into the animation?
		float TimeInTicks = myAnimationTimePrev * TicksPerSecond;
		// Where are we in the animation, on which frame?
		float AnimationTime0 = fmodf(TimeInTicks, static_cast<float>(myAnimations[myPrevAnimIndex]->mAnimations[0]->mDuration));
	// !  Animation time for the first anim ( 0 )
		
	// Animation time for the second anim ( 1 ) (Current?)
		TicksPerSecond = 
			static_cast<float>(myAnimations[myCurAnimIndex]->mAnimations[0]->mTicksPerSecond) != 0 
			?
			static_cast<float>(myAnimations[myCurAnimIndex]->mAnimations[0]->mTicksPerSecond) : 25.0f;
		
		TimeInTicks = myAnimationTimeCurrent * TicksPerSecond;
		float AnimationTime1 = fmodf(TimeInTicks, static_cast<float>(myAnimations[myCurAnimIndex]->mAnimations[0]->mDuration));
	// ! Animation time for the second anim ( 1 )
		
		ReadNodeHeirarchy(	myAnimations[myPrevAnimIndex], myAnimations[myCurAnimIndex]
						  , AnimationTime0, AnimationTime1
						  , myAnimations[myPrevAnimIndex]->mRootNode, myAnimations[myCurAnimIndex]->mRootNode
						  , Identity, 2);//stopAnimLevel=2
	}
	else// There is only one animation to play. No blending.
	{
		float AnimationTime = 0.0f;
		if (myAnimations[myCurAnimIndex]->mAnimations)
		{
			float TicksPerSecond = 
				static_cast<float>(myAnimations[myCurAnimIndex]->mAnimations[0]->mTicksPerSecond) != 0 
				? 
				static_cast<float>(myAnimations[myCurAnimIndex]->mAnimations[0]->mTicksPerSecond) : 25.0f;
			float TimeInTicks = myAnimationTimePrev * TicksPerSecond;
			AnimationTime = fmodf(TimeInTicks, static_cast<float>(myAnimations[myCurAnimIndex]->mAnimations[0]->mDuration));
		}

		ReadNodeHeirarchy(myAnimations[myCurAnimIndex], AnimationTime, myAnimations[myCurAnimIndex]->mRootNode, Identity, 2);
	}

	aTransformsVector.resize(myNumOfBones);

	for (uint i = 0; i < myNumOfBones; i++)
	{
		aTransformsVector[i] = myBoneInfo[i].myFinalTransformation;
	}
}

void CAnimationController::UpdateAnimationTimes()
{
	float dt = CTimer::Dt() * 50.0f;

	myAnimationTimePrev += dt;
	if (myBlendingTime > 0.f)
	{
		myBlendingTime -= dt * myBlendingTimeMul;
		if (myBlendingTime <= 0.f)
		{
			myAnimationTimePrev = myAnimationTimeCurrent;
		}
		if (myUpdateBoth)
		{
			myAnimationTimeCurrent += dt;
		}
	}
	else
	{
		myAnimationTimeCurrent += dt;
	}

	if (myTemporary)
	{
		myPlayTime -= dt;
		if (myPlayTime <= 0.f)
		{
			myTemporary = false;
			SetAnimIndex(myPrevAnimIndex);
		}
	}
}

bool CAnimationController::SetAnimIndex(uint index, bool updateBoth, float blendDuration, bool temporary, float time)
{
	if (index == static_cast<uint>(myCurAnimIndex) || index >= static_cast<uint>(myAnimations.size()))
	{
		return false;
	}
	myPrevAnimIndex = myCurAnimIndex;
	myCurAnimIndex = index;
	myBlendingTime = blendDuration;
	myBlendingTimeMul = blendDuration > 0.0f ? 1.0f / blendDuration : 1.0f;
	myAnimationTimeCurrent = 0.f;
	myUpdateBoth = updateBoth;
	myTemporary = temporary;
	myPlayTime = time;
	return true;
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