#pragma once
#define EPSILON 0.0001f

class CTerrainMesh;

class CTerrainGenerator
{
public:
	void InitMesh(const int aMeshSize, const int aNumberOfMeshes);
	void GenerateTerrain(CTerrainMesh& aTerrainMesh, const int anOffset);
	void GenerateFile();
	const std::vector<CTerrainMesh*> TerrainMeshes() const;
	bool CheckIfOverlap(Vector3 aFirstPosition, Vector3 aSecondPosition);

private:
	std::vector<CTerrainMesh*> myTerrainMeshes;
};

