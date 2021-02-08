#pragma once


class CScene;
class CGameObject;

class CUnityFactory
{
public:
	CUnityFactory();
	~CUnityFactory();

	//bool FillScene(const SLoadScreenData& aData, const std::vector<std::string>& aBinModelPaths, CScene& aScene);
	//bool FillScene(const SInGameData& aData, const std::vector<std::string>& aBinModelPaths, CScene& aScene);

private:
	//CGameObject* CreateGameObject(const SCameraData& aData, bool addCameraController = true);
	//CGameObject* CreateGameObject(const SDirectionalLightData& aData);
	//CGameObject* CreateGameObject(const SPointLightData& aData);
	//CGameObject* CreateGameObject(const SGameObjectData& aData, const std::string& aModelPath);
	CGameObject* CreateGameObjectInstanced(const std::string& aModelPath, int aInstancedID, std::vector<DirectX::SimpleMath::Matrix> aInstancedTransforms);
};
