#pragma once
#include "NodeType.h"

class CNodeTypeEnemySpawn : public CNodeType
{
public:
	CNodeTypeEnemySpawn();
	int OnEnter(class CNodeInstance* aTriggeringNodeInstance) override;
	std::string GetNodeTypeCategory() override { return "Enemy"; }
private:
	static void SpawnEnemies(const int aNumber, const Vector3& aSpawnPosition);
};

