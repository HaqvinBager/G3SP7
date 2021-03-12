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

const Vector3 SVertex::VertexPosition() const
{
	return myVertexPosition;
}

void SVertex::VertexNormal(const Vector3 aVertexNormal)
{
	myVertexNormal = aVertexNormal;
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

void CTerrainMesh::GenerateVertexNormals(const SFace aFace)
{
}

std::vector<SVertex> CTerrainMesh::Vertices()
{
	return myVertices;
}

void CTerrainMesh::Vertices(std::vector<SVertex> someVertices)
{
	myVertices = someVertices;
}

const std::vector<SFace> CTerrainMesh::Faces() const
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