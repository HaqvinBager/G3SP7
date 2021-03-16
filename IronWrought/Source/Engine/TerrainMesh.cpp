#include "stdafx.h"
#include "TerrainMesh.h"

SVertex::SVertex() : myVertexPosition(0.0f, 0.0f, 0.0f), myVertexNormal(0.0f, 0.0f, 0.0f)
{
}

SVertex::SVertex(const Vector3 aVertexPosition) : myVertexPosition(aVertexPosition), myVertexNormal(0.0f,0.0f,0.0f)
{
}

void SVertex::VertexPosition(const Vector3 aVertexPosition)
{
	myVertexPosition = aVertexPosition;
}

void SVertex::VertexYPosition(const float aYPosition)
{
	myVertexPosition.y = aYPosition;
}

const Vector3 SVertex::VertexPosition() const
{
	return myVertexPosition;
}

void SVertex::VertexNormal(const Vector3 aVertexNormal)
{
	myVertexNormal = aVertexNormal;
	myVertexNormal.Normalize();
}

const Vector3 SVertex::VertexNormal() const
{
	return myVertexNormal;
}

SFace::SFace()
{
	myVertexIndeces.resize(3);
}

SFace::~SFace()
{
	myVertexIndeces.clear();
}

void SFace::SetVertexIndexAtIndex(const int anIndex, const int aVertexIndex)
{
	myVertexIndeces[anIndex] = aVertexIndex;
}

const std::vector<int> SFace::GetIndeces() const
{
	return myVertexIndeces;
}

CTerrainMesh::CTerrainMesh() : myMeshSize(0), myFaceWidth(0)
{
}

CTerrainMesh::~CTerrainMesh()
{
	myVertices.clear();
	myFaces.clear();
}

void CTerrainMesh::GenerateVertexNormals(SFace& aFace)
{
	Vector3 U = myVertices[aFace.GetIndeces()[2] - 1].VertexPosition();
	U *= myVertices[aFace.GetIndeces()[1] - 1].VertexPosition();
	Vector3 V = myVertices[aFace.GetIndeces()[2] - 1].VertexPosition();
	V *= myVertices[aFace.GetIndeces()[0] - 1].VertexPosition();

	Vector3 result = U.Cross(V);
	result.Normalize();
	result.x = fabs(result.x);
	result.y = fabs(result.y);
	result.z = fabs(result.z);

	myVertices[aFace.GetIndeces()[0] - 1].VertexNormal(myVertices[aFace.GetIndeces()[2] - 1].VertexNormal() + result);
	myVertices[aFace.GetIndeces()[1] - 1].VertexNormal(myVertices[aFace.GetIndeces()[2] - 1].VertexNormal() + result);
	myVertices[aFace.GetIndeces()[2] - 1].VertexNormal(myVertices[aFace.GetIndeces()[2] - 1].VertexNormal() + result);
}

std::vector<SVertex> CTerrainMesh::Vertices()
{
	return myVertices;
}

void CTerrainMesh::Vertices(std::vector<SVertex> someVertices)
{
	myVertices = someVertices;
}

std::vector<SFace> CTerrainMesh::Faces()
{
	return myFaces;
}

void CTerrainMesh::Faces(const std::vector<SFace> someFaces)
{
	myFaces = someFaces;
}

const int CTerrainMesh::MeshSize() const
{
	return myMeshSize;
}

void CTerrainMesh::MeshSize(const int aGridSize)
{
	myMeshSize = aGridSize;
	myFaceWidth = 1.0f / static_cast<float>(myMeshSize);
}

const float CTerrainMesh::FaceWidth() const
{
	return myFaceWidth;
}