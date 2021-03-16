#pragma once

struct SVertex
{
public:
	SVertex();
	SVertex(const Vector3 aVertexPosition);
	void VertexPosition(const Vector3 aVertexPosition);
	void VertexYPosition(const float aYPosition);
	const Vector3 VertexPosition() const;
	void VertexNormal(const Vector3 aVertexNormal);
	const Vector3 VertexNormal() const;

private:
	Vector3 myVertexPosition;
	Vector3 myVertexNormal;
};

struct SFace
{
public:
	SFace();
	~SFace();

	void SetVertexIndexAtIndex(const int anIndex, const int aVertexIndex);
	const std::vector<int> GetIndeces() const;

private:
	std::vector<int> myVertexIndeces;
};

class CTerrainMesh
{
public:
	CTerrainMesh();
	~CTerrainMesh();

	void GenerateVertexNormals(SFace& aFace);

	std::vector<SVertex> Vertices();
	void Vertices(std::vector<SVertex> someVertices);
	std::vector<SFace> Faces();
	void Faces(const std::vector<SFace> someFaces);
	const int MeshSize() const;
	void MeshSize(const int aGridSize);
	const float FaceWidth() const;

private:
	std::vector<SVertex> myVertices;
	std::vector<SFace> myFaces;
	int myMeshSize;
	float myFaceWidth;
};