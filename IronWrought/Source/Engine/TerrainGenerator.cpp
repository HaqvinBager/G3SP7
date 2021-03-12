#include "stdafx.h"
#include "TerrainGenerator.h"
#include "TerrainMesh.h"
#include <fstream>
#include "PerlinNoise.h"

void CTerrainGenerator::InitMesh(const int aMeshSize, const int aNumberOfMeshes)
{
    int size = aNumberOfMeshes * aNumberOfMeshes;
    myTerrainMeshes.resize(size);

    for (int i = 0; i < myTerrainMeshes.size(); ++i)
    {
        myTerrainMeshes[i] = new CTerrainMesh();
        myTerrainMeshes[i]->MeshSize(aMeshSize);
        GenerateTerrain(*myTerrainMeshes[i], i);
    }
}

void CTerrainGenerator::GenerateTerrain(CTerrainMesh& aTerrainMesh, const int anOffset)
{
	int faceOffset = 0;
	int vertexOffset = 0;
	int offset = 0;
	int offsetX = 0;
	int offsetZ = 0;

	int x1 = 0;
	int x2 = 0;
	int z1 = 0;
	int z2 = 0;

	int index = 0;
	int index1 = 0;
	int index2 = 0;
	int index3 = 0;
	int index4 = 0;
	
	int meshSizeSquared = aTerrainMesh.MeshSize() * aTerrainMesh.MeshSize();

	std::vector<SFace> faces;
	faces.resize(meshSizeSquared * 2);

	std::vector<SVertex> vertices;
	vertices.resize(meshSizeSquared * 4);

	std::vector<SVertex> verticesToAdd;
	verticesToAdd.resize(5);

	SFace face;
	for (unsigned int i = 0; i < aTerrainMesh.MeshSize(); ++i)
	{
		for (unsigned int j = 0; j < aTerrainMesh.MeshSize(); ++j)
		{
			if (anOffset % 2 == 0)
			{
				offsetX = 0;
				offsetZ = aTerrainMesh.MeshSize() * anOffset;
			}
			else
			{
				offsetX = aTerrainMesh.MeshSize() * anOffset;
				offsetZ = 0;
			}

			offset = (aTerrainMesh.MeshSize() * i) + j;
			x1 = (aTerrainMesh.FaceWidth()) * i + offsetX;
			x2 = (aTerrainMesh.FaceWidth()) * (i + 1) + offsetX;
			z1 = (aTerrainMesh.FaceWidth()) * j + offsetZ;
			z2 = (aTerrainMesh.FaceWidth()) * (j + 1) + offsetZ;
			verticesToAdd[0].VertexPosition({ x1, 0.0f, z1 });
			verticesToAdd[1].VertexPosition({ x2, 0.0f, z1 });
			verticesToAdd[2].VertexPosition({ x1, 0.0f, z2 });
			verticesToAdd[3].VertexPosition({ x2, 0.0f, z2 });
			index1 = 0;
			index2 = 0;
			index3 = 0;
			index4 = 0;

			if (i == 0 && j == 0)
			{
				index1 = 1;
				index2 = 2;
				index3 = 3;
				index4 = 4;
				vertices[offset + vertexOffset] = verticesToAdd[0];
				vertexOffset++;
				vertices[offset + vertexOffset] = verticesToAdd[1];
				vertexOffset++;
				vertices[offset + vertexOffset] = verticesToAdd[2];
				vertexOffset++;
				vertices[offset + vertexOffset] = verticesToAdd[3];
			}
			else
			{
				for (unsigned int l = 0; l < verticesToAdd.size(); ++l)
				{
					index = 0;
					for (unsigned int k = 0; k < offset + vertexOffset + 1; ++k)
					{
						if (CheckIfOverlap(vertices[k].VertexPosition(), verticesToAdd[l].VertexPosition()))
						{
							index = k + 1;
							break;
						}
					}

					if (l == 0)
					{
						if (index == 0)
						{
							vertices[offset + vertexOffset] = verticesToAdd[l];
							vertexOffset++;
							index1 = offset + vertexOffset;
						}
						else
							index1 = index;
					}
					else if (l == 1)
					{
						if (index == 0)
						{
							vertices[offset + vertexOffset] = verticesToAdd[l];
							vertexOffset++;
							index2 = offset + vertexOffset;
						}
						else
							index2 = index;
					}
					else if (l == 2)
					{
						if (index == 0)
						{
							vertices[offset + vertexOffset] = verticesToAdd[l];
							vertexOffset++;
							index3 = offset + vertexOffset;
						}
						else
							index3 = index;
					}
					else if (l == 3)
					{
						if (index == 0)
						{
							vertices[offset + vertexOffset] = verticesToAdd[l];
							index4 = offset + vertexOffset + 1;
						}
						else
							index4 = index;
					}
				}
			}

			if (index1 % 2 == 0)
			{
				face.SetVertexIndexAtIndex(0, index1);
				face.SetVertexIndexAtIndex(1, index2);
				face.SetVertexIndexAtIndex(2, index3);
				faces[offset + faceOffset] = face;
				faceOffset++;
				face.SetVertexIndexAtIndex(0, index2);
				face.SetVertexIndexAtIndex(1, index3);
				face.SetVertexIndexAtIndex(2, index4);
				faces[offset + faceOffset] = face;
			}
			else if (index1 % 2 == 1)
			{
				face.SetVertexIndexAtIndex(0, index1);
				face.SetVertexIndexAtIndex(1, index2);
				face.SetVertexIndexAtIndex(2, index4);
				faces[offset + faceOffset] = face;
				faceOffset++;
				face.SetVertexIndexAtIndex(0, index1);
				face.SetVertexIndexAtIndex(1, index3);
				face.SetVertexIndexAtIndex(2, index4);
				faces[offset + faceOffset] = face;
			}
		}
	}
	faces.resize(meshSizeSquared + faceOffset);
	aTerrainMesh.Faces(faces);
	vertices.resize(meshSizeSquared + vertexOffset);
	aTerrainMesh.Vertices(vertices);

	PerlinNoise* perlinNoise = new PerlinNoise(std::time(NULL));
	float yPosition;
	for (auto& position : aTerrainMesh.Vertices())
	{
		yPosition = perlinNoise->noise(position.VertexPosition().x, position.VertexPosition().y, position.VertexPosition().z) * 0.75f;
		position.VertexPosition({ position.VertexPosition().x, yPosition, position.VertexPosition().z });
	}
}

void CTerrainGenerator::GenerateFile()
{
	std::ofstream outfile("test.txt");
	for (int i = 0; i < myTerrainMeshes.size(); ++i)
	{
		for (auto& position : myTerrainMeshes[i]->Vertices())
		{
			outfile << "v " << position.VertexPosition().x << " " << position.VertexPosition().y << " " << position.VertexPosition().z << "\n";
		}
	}

	for (int j = 0; j < myTerrainMeshes.size(); ++j)
	{
		for (int i = 0; i < myTerrainMeshes[j]->Faces().size(); ++i)
		{
			outfile << "f " << myTerrainMeshes[j]->Faces()[i].GetIndeces()[0] << " " << myTerrainMeshes[j]->Faces()[i].GetIndeces()[1] << " " << myTerrainMeshes[j]->Faces()[i].GetIndeces()[2] << "\n";
		}
	}
	outfile.close();
}

const std::vector<CTerrainMesh*> CTerrainGenerator::TerrainMeshes() const
{
    return myTerrainMeshes;
}

bool CTerrainGenerator::CheckIfOverlap(Vector3 aFirstPosition, Vector3 aSecondPosition)
{
	float xDifference = fabs(aFirstPosition.x - aSecondPosition.x);
	float yDifference = fabs(aFirstPosition.y - aSecondPosition.y);
	float zDifference = fabs(aFirstPosition.z - aSecondPosition.z);
	if (xDifference > EPSILON)
		return false;
	if (yDifference > EPSILON)
		return false;
	if (zDifference > EPSILON)
		return false;

	return true;
}
