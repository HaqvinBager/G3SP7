#pragma once
#include <filesystem>
#include <string>
#include <vector>

class CGameObject;
class CAnimationComponent;
namespace AnimationLoader
{
	// Checks if aGameObject has a CModelComponent, and CModel HasBones() is true.
	bool ShouldLoadAnimations(const CGameObject* aGameObject);
	// Checks for /Animations/ folder in models folder, returns any FBX:es found.
	std::vector<std::string> GetAnimationPaths(const std::string& aModelPath);
	// Checks if CModel HasBones is true, gets animation FBX:es from /Animations/ folder and creates and adds CAnimationComponent to aGameObject.
	CAnimationComponent* AddAnimationsToGameObject(CGameObject* aGameObject, const std::string& aModelPath);
}
namespace AnimationLoaderInternal
{
	std::size_t number_of_files_in_directory(const std::filesystem::path& path);
}
