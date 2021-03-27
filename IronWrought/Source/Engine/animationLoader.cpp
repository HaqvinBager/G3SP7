#include "stdafx.h"
#include "animationLoader.h"

#include "GameObject.h"
#include "ModelComponent.h"
#include "Model.h"
#include "AnimationComponent.h"
#include "AnimationController.h"

namespace AnimationLoader
{
	bool ShouldLoadAnimations(const CGameObject* aGameObject)
	{
		if(aGameObject->GetComponent<CModelComponent>())
			return (aGameObject->GetComponent<CModelComponent>()->GetMyModel()->HasBones());

		assert(false && "GameObject has no CModelComponent");
		return false;
	}

	std::vector<std::string> GetAnimationPaths(const std::string& aModelPath)
	{
		size_t lastSlashIndex = aModelPath.find_last_of("\\/");
		std::string animationPath = aModelPath.substr(0, aModelPath.length() - (aModelPath.length() - lastSlashIndex - 1));
		animationPath.append("Animations/");

		std::filesystem::directory_iterator start(animationPath.c_str());
		//try
		//{
		//	throw start = std::filesystem::directory_iterator(animationPath.c_str());
		//}
		//catch (int exceptionNr)
		//{
		//	animationPath.append(" - Does not exist! " + std::to_string(exceptionNr));
		//	assert(false && animationPath.c_str());
		//	return std::vector<std::string>();
		//}

		std::vector<std::string> animsInFolder;
		animsInFolder.reserve(AnimationLoaderInternal::number_of_files_in_directory(start->path().parent_path()));
		for (auto& file : start)
		{
			auto& itPath = file.path();
			if (!itPath.has_extension())
				continue;
			if(itPath.extension().filename().string() == ".fbx")
				animsInFolder.emplace_back(animationPath + itPath.filename().string());
		}
		return std::move(animsInFolder);
	}

	CAnimationComponent* AddAnimationsToGameObject(CGameObject* aGameObject, const std::string& aModelPath)
	{
		if (!ShouldLoadAnimations(aGameObject))
		{
			assert(false && "Trying to add CAnimationComponent to GameObject that holds no CModel with bones!");
			return nullptr;
		}

		std::vector<std::string> someAnimations = GetAnimationPaths(aModelPath);
		return aGameObject->AddComponent<CAnimationComponent>(*aGameObject, aModelPath, someAnimations);;
		// No assert or return false here: Base model is Tpose and can still be used to render the model.

		//CAnimationComponent* anim = 
		
		//CBlendTree* blendTree = anim->GetController()->AddBlendTree("Locomotion");
		//for (const auto& motion : anim->GetController()->GetMotionNames())
		//	blendTree->AddMotion(motion);

	}
}
namespace AnimationLoaderInternal
{
	std::size_t number_of_files_in_directory(const std::filesystem::path& path)
	{// from : https://stackoverflow.com/questions/41304891/how-to-count-the-number-of-files-in-a-directory-using-standard/41305019
		using std::filesystem::directory_iterator;
		return std::distance(directory_iterator(path), directory_iterator{});
	}
}