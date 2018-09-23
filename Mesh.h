#pragma once
#include "DXCore.h"
#include "Vertex.h"

class Mesh
{
public:
	Mesh(Vertex* vertexArray, int numVerts, unsigned int * indexArray, int numIndices, ID3D11Device* device);
	Mesh(const char* objFile, ID3D11Device* device);
	~Mesh();

	ID3D11Buffer* GetVertexBuffer();
	ID3D11Buffer* GetIndexBuffer();
	int GetIndexCount();

private:
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;
	int numIndices;

	void CalculateTangents(Vertex* vertexArray, int numVerts, unsigned int* indexArray, int numIndices);
	void CreateBuffers(Vertex* vertexArray, int numVerts, unsigned int * indexArray, int numIndices, ID3D11Device* device);


};

