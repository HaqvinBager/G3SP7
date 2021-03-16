#pragma once
#define EPSILON 0.0001f

class CTerrainMesh;
class PerlinNoise;

class CTerrainGenerator
{
public:
	void InitMesh(const int aMeshSize);
	void InitMeshes(const int aMeshSize, const int aGridWidth);
	void GenerateTerrain();
	void GenerateTerrains();
	void GenerateFile();
	void GenerateFiles();
	const CTerrainMesh* TerrainMesh() const;
	const std::vector<CTerrainMesh*> TerrainMeshes() const;
	bool CheckIfOverlap(Vector3 aFirstPosition, Vector3 aSecondPosition);
	//HRESULT RenderTerrain();
private:
	int myGridWidth;
	CTerrainMesh* myTerrainMesh;
	std::vector<CTerrainMesh*> myTerrainMeshes;
	PerlinNoise* myPerlinNoise;
};

