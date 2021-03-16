#include "stdafx.h"
#include "TerrainGenerator.h"
#include "TerrainMesh.h"
#include <fstream>
#include "PerlinNoise.h"

void CTerrainGenerator::InitMesh(const int aMeshSize)
{
	myPerlinNoise = new PerlinNoise();
	myTerrainMesh = new CTerrainMesh();
	myTerrainMesh->MeshSize(aMeshSize);
	GenerateTerrain();
}

void CTerrainGenerator::InitMeshes(const int aMeshSize, const int aGridWidth)
{
	myPerlinNoise = new PerlinNoise();
	myGridWidth = aGridWidth;
	myTerrainMeshes.resize(myGridWidth * myGridWidth);
	for (int i = 0; i < myTerrainMeshes.size(); ++i)
	{
		myTerrainMeshes[i] = new CTerrainMesh();
		myTerrainMeshes[i]->MeshSize(aMeshSize);
	}
	GenerateTerrains();
}

void CTerrainGenerator::GenerateTerrain()
{
	int gridOffset = 0;
	int faceOffset = 0;
	int vertexOffset = 0;
	int offset = 0;
	//int offsetX = 0;
	//int offsetZ = 0;

	float x1 = 0;
	float x2 = 0;
	float z1 = 0;
	float z2 = 0;

	int index = 0;
	int index1 = 0;
	int index2 = 0;
	int index3 = 0;
	int index4 = 0;

	int meshSizeSquared = myTerrainMesh->MeshSize() * myTerrainMesh->MeshSize();

	std::vector<SFace> faces;
	faces.resize(meshSizeSquared * 2);

	std::vector<SVertex> vertices;
	vertices.resize(meshSizeSquared * 4);

	std::vector<SVertex> verticesToAdd;
	verticesToAdd.resize(5);

	SFace face;
	for (int i = 0; i < myTerrainMesh->MeshSize(); ++i)
	{
		gridOffset = faceOffset + i;
		for (int j = 0; j < myTerrainMesh->MeshSize(); ++j)
		{
			++gridOffset;

			offset = (myTerrainMesh->MeshSize() * i) + j;
			x1 = myTerrainMesh->FaceWidth() * static_cast<float>(i);
			x2 = myTerrainMesh->FaceWidth() * static_cast<float>((i + 1));
			z1 = myTerrainMesh->FaceWidth() * static_cast<float>(j);
			z2 = myTerrainMesh->FaceWidth() * static_cast<float>((j + 1));
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
				for (size_t l = 0; l < verticesToAdd.size(); ++l)
				{
					index = 0;
					for (int k = 0; k < offset + vertexOffset + 1; ++k)
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

			if (gridOffset % 2 == 0)
			{
				face.SetVertexIndexAtIndex(0, index3);
				face.SetVertexIndexAtIndex(1, index2);
				face.SetVertexIndexAtIndex(2, index1);
				faces[offset + faceOffset] = face;
				faceOffset++;
				face.SetVertexIndexAtIndex(0, index2);
				face.SetVertexIndexAtIndex(1, index3);
				face.SetVertexIndexAtIndex(2, index4);
				faces[offset + faceOffset] = face;
			}
			else if (gridOffset % 2 == 1)
			{
				face.SetVertexIndexAtIndex(0, index4);
				face.SetVertexIndexAtIndex(1, index2);
				face.SetVertexIndexAtIndex(2, index1);
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
	myTerrainMesh->Faces(faces);

	float yPosition;
	for (int i = 0; i < vertices.size(); ++i)
	{
		yPosition = static_cast<float>(myPerlinNoise->noise(vertices[i].VertexPosition().x, vertices[i].VertexPosition().y, vertices[i].VertexPosition().z));

		vertices[i].VertexYPosition(yPosition);
	}

	vertices.resize(meshSizeSquared + vertexOffset);
	myTerrainMesh->Vertices(vertices);


	//for (int i = 0; i < myTerrainMesh->Faces().size(); ++i)
	//{
	//	myTerrainMesh->GenerateVertexNormals(myTerrainMesh->Faces()[i]);
	//}
}

void CTerrainGenerator::GenerateTerrains()
{
	int gridOffset = 0;
	int faceOffset = 0;
	int vertexOffset = 0;
	int offset = 0;
	int indexOffset = 0;
	//int offsetX = 0;
	//int offsetZ = 0;

	float x1 = 0;
	float x2 = 0;
	float z1 = 0;
	float z2 = 0;

	int index = 0;
	int index1 = 0;
	int index2 = 0;
	int index3 = 0;
	int index4 = 0;

	int meshSizeSquared = myTerrainMeshes[0]->MeshSize() * myTerrainMeshes[0]->MeshSize();

	std::vector<SFace> faces;
	faces.resize(meshSizeSquared * 2);

	std::vector<SVertex> vertices;
	vertices.resize(meshSizeSquared * 4);

	std::vector<SVertex> verticesToAdd;
	verticesToAdd.resize(5);

	SFace face;
	int xOffset = 0;
	int zOffset = 0;
	for (int i = 0; i < myTerrainMeshes.size(); ++i)
	{
		xOffset = i / myGridWidth;
		zOffset = i % myGridWidth;
		vertexOffset = 0;
		faceOffset = 0;
		for (int j = 0; j < myTerrainMeshes[i]->MeshSize(); ++j)
		{
			gridOffset = faceOffset + j;
			for (int k = 0; k < myTerrainMeshes[i]->MeshSize(); ++k)
			{
				++gridOffset;

				offset = (myTerrainMeshes[i]->MeshSize() * j) + k;
				x1 = xOffset + (myTerrainMeshes[i]->FaceWidth() * static_cast<float>(j));
				x2 = xOffset + (myTerrainMeshes[i]->FaceWidth() * static_cast<float>((j + 1)));
				z1 = zOffset + (myTerrainMeshes[i]->FaceWidth() * static_cast<float>(k));
				z2 = zOffset + (myTerrainMeshes[i]->FaceWidth() * static_cast<float>((k + 1)));
				verticesToAdd[0].VertexPosition({ x1, 0.0f, z1 });
				verticesToAdd[1].VertexPosition({ x2, 0.0f, z1 });
				verticesToAdd[2].VertexPosition({ x1, 0.0f, z2 });
				verticesToAdd[3].VertexPosition({ x2, 0.0f, z2 });
				index1 = 0;
				index2 = 0;
				index3 = 0;
				index4 = 0;

				if (j == 0 && k == 0)
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
					for (size_t l = 0; l < verticesToAdd.size(); ++l)
					{
						index = 0;
						for (int m = 0; m < offset + vertexOffset + 1; ++m)
						{
							if (CheckIfOverlap(vertices[m].VertexPosition(), verticesToAdd[l].VertexPosition()))
							{
								index = m + 1;
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

				if (gridOffset % 2 == 0)
				{
					face.SetVertexIndexAtIndex(0, index3 + indexOffset);
					face.SetVertexIndexAtIndex(1, index2 + indexOffset);
					face.SetVertexIndexAtIndex(2, index1 + indexOffset);
					faces[offset + faceOffset] = face;
					faceOffset++;
					face.SetVertexIndexAtIndex(0, index2 + indexOffset);
					face.SetVertexIndexAtIndex(1, index3 + indexOffset);
					face.SetVertexIndexAtIndex(2, index4 + indexOffset);
					faces[offset + faceOffset] = face;
				}
				else if (gridOffset % 2 == 1)
				{
					face.SetVertexIndexAtIndex(0, index4 + indexOffset);
					face.SetVertexIndexAtIndex(1, index2 + indexOffset);
					face.SetVertexIndexAtIndex(2, index1 + indexOffset);
					faces[offset + faceOffset] = face;
					faceOffset++;
					face.SetVertexIndexAtIndex(0, index1 + indexOffset);
					face.SetVertexIndexAtIndex(1, index3 + indexOffset);
					face.SetVertexIndexAtIndex(2, index4 + indexOffset);
					faces[offset + faceOffset] = face;
				}
			}
		}
		faces.resize(meshSizeSquared + faceOffset);
		myTerrainMeshes[i]->Faces(faces);

		float yPosition;
		for (int j = 0; j < vertices.size(); ++j)
		{
			yPosition = static_cast<float>(myPerlinNoise->noise(vertices[j].VertexPosition().x, vertices[j].VertexPosition().y, vertices[j].VertexPosition().z));

			vertices[j].VertexYPosition(yPosition);
		}

		vertices.resize(meshSizeSquared + vertexOffset);
		myTerrainMeshes[i]->Vertices(vertices);

		indexOffset += offset + vertexOffset + 1;
	}
}

void CTerrainGenerator::GenerateFile()
{
	std::ofstream outfile("test.obj");

	if (!outfile)
	{
		std::cerr << "Cannot open the output file." << std::endl;
	}

	for (auto& position : myTerrainMesh->Vertices())
	{
		outfile << "v " << position.VertexPosition().x << " " << position.VertexPosition().y << " " << position.VertexPosition().z << "\n";
	}

	//for (auto& position : myTerrainMesh->Vertices())
	//{
	//	outfile << "vn " << position.VertexNormal().x << " " << position.VertexNormal().y << " " << position.VertexNormal().z << "\n";
	//}

	for (int i = 0; i < myTerrainMesh->Faces().size(); ++i)
	{
		outfile << "f " << myTerrainMesh->Faces()[i].GetIndeces()[0] << " " << myTerrainMesh->Faces()[i].GetIndeces()[1] << " " << myTerrainMesh->Faces()[i].GetIndeces()[2] << "\n";
	}

	outfile.close();
}

void CTerrainGenerator::GenerateFiles()
{
	std::ofstream outfile("test.obj");

	if (!outfile)
	{
		std::cerr << "Cannot open the output file." << std::endl;
	}

	for (int i = 0; i < myTerrainMeshes.size(); ++i)
	{
		for (auto& position : myTerrainMeshes[i]->Vertices())
		{
			outfile << "v " << position.VertexPosition().x << " " << position.VertexPosition().y << " " << position.VertexPosition().z << "\n";
		}
	}

	//for (int i = 0; i < myTerrainMeshes.size(); ++i)
	//{
	//	for (auto& position : myTerrainMeshes[i]->Vertices())
	//	{
	//		outfile << "vn " << position.VertexNormal().x << " " << position.VertexNormal().y << " " << position.VertexNormal().z << "\n";
	//	}
	//}

	for (int i = 0; i < myTerrainMeshes.size(); ++i)
	{
		for (int j = 0; j < myTerrainMeshes[i]->Faces().size(); ++j)
		{
			outfile << "f " << myTerrainMeshes[i]->Faces()[j].GetIndeces()[0] << " " << myTerrainMeshes[i]->Faces()[j].GetIndeces()[1] << " " << myTerrainMeshes[i]->Faces()[j].GetIndeces()[2] << "\n";
		}
	}

	outfile.close();
}

const CTerrainMesh* CTerrainGenerator::TerrainMesh() const
{
	return myTerrainMesh;
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

//HRESULT CTerrainGenerator::RenderTerrain()
//{
//	HRESULT hr;
//	// Pack all the vertices into vertex buffer
//	D3D11_BUFFER_DESC vbd;
//	vbd.Usage = D3D11_USAGE_IMMUTABLE;
//	vbd.ByteWidth = sizeof(Vector3) * myTerrainMeshes[0]->Vertices().size();
//	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
//	vbd.CPUAccessFlags = 0;
//	vbd.MiscFlags = 0;
//	D3D11_SUBRESOURCE_DATA vinitData;
//	vinitData.pSysMem = &(myTerrainMeshes[0]->Vertices()[0]);
//	m_pGraphics->getDevice()->CreateBuffer(&vbd, &vinitData, &mVB);
//
//	// Pack the indices of all the meshes into one index buffer.
//	D3D11_BUFFER_DESC ibd;
//	ibd.Usage = D3D11_USAGE_DEFAULT;
//	ibd.ByteWidth = sizeof(UINT) * myTerrainMeshes.size();
//	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
//	ibd.CPUAccessFlags = 0;
//	ibd.MiscFlags = 0;
//	D3D11_SUBRESOURCE_DATA iinitData;
//	iinitData.pSysMem = &myTerrainMeshes[0];
//	m_pGraphics->getDevice()->CreateBuffer(&ibd, &iinitData, &mIB);
//
//	// Create the constant buffer
//	ibd.Usage = D3D11_USAGE_DEFAULT;
//	ibd.ByteWidth = sizeof(ConstantBuffer);
//	ibd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
//	ibd.CPUAccessFlags = 0;
//	hr = m_pGraphics->getDevice()->CreateBuffer(&ibd, nullptr, &m_pConstantBuffer);
//	if (FAILED(hr))
//		return hr;
//
//	return hr;
//}
//}
