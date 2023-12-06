// plane mesh
// Quad mesh made of many quads. Default is 100x100
#include "TessellationPlane.h"

// Initialise buffer and load texture.
TessellationPlane::TessellationPlane(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int lresolution)
{
	resolution = lresolution;
	initBuffers(device);
}

// Release resources.
TessellationPlane::~TessellationPlane()
{
	// Run parent deconstructor
	BaseMesh::~BaseMesh();
}

// Generate plane (including texture coordinates and normals).
void TessellationPlane::initBuffers(ID3D11Device* device)
{
	VertexType* vertices;
	unsigned long* indices;
	int index;
	float positionX, positionZ, u, v, step;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;

	// Calculate the number of vertices in the terrain mesh.
	vertexCount = (resolution - 1) * (resolution - 1) * 6;

	indexCount = vertexCount;
	vertices = new VertexType[vertexCount];
	indices = new unsigned long[indexCount];

	// UV coords.
	u = 0;
	v = 0;
	step = 1.0f / resolution;

	// Loop for each line of vertices
	for (int o = 0; o < (resolution - 1); o++)
	{
		for (int i = 0; i < (resolution - 1); i++)
		{
			int verticeIndex = (i * resolution + o) * 4;
			int indiceIndex = (i * resolution + o) * 4;

			// lower left (upper left)
			positionX = (float)(i);
			positionZ = (float)(o + 1);

			vertices[verticeIndex].position = XMFLOAT3(positionX, 0.0f, positionZ);
			vertices[verticeIndex].texture = XMFLOAT2(u, v + step);
			vertices[verticeIndex].normal = XMFLOAT3(0.0, 1.0, 0.0);
			indices[indiceIndex] = verticeIndex;

			// Upper left. (bottom left)
			positionX = (float)(i);
			positionZ = (float)(o);

			vertices[verticeIndex + 1].position = XMFLOAT3(positionX, 0.0f, positionZ);
			vertices[verticeIndex + 1].texture = XMFLOAT2(u, v);
			vertices[verticeIndex + 1].normal = XMFLOAT3(0.0, 1.0, 0.0);
			indices[indiceIndex + 1] = verticeIndex + 1;

			// Bottom right (upper right)
			positionX = (float)(i + 1);
			positionZ = (float)(o);

			vertices[verticeIndex + 2].position = XMFLOAT3(positionX, 0.0f, positionZ);
			vertices[verticeIndex + 2].texture = XMFLOAT2(u + step, v);
			vertices[verticeIndex + 2].normal = XMFLOAT3(0.0, 1.0, 0.0);
			indices[indiceIndex + 2] = verticeIndex + 2;

			// Upper right. (bottom right)
			positionX = (float)(i + 1);
			positionZ = (float)(o + 1);

			vertices[verticeIndex + 3].position = XMFLOAT3(positionX, 0.0f, positionZ);
			vertices[verticeIndex + 3].texture = XMFLOAT2(u + step, v + step);
			vertices[verticeIndex + 3].normal = XMFLOAT3(0.0, 1.0, 0.0);
			indices[indiceIndex + 3] = verticeIndex + 3;

			u += step;
		}
		u = 0;
		v += step;
	}

	// Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;
	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;
	// Now create the vertex buffer.
	device->CreateBuffer(&vertexBufferDesc, &vertexData, &vertexBuffer);

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;
	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;
	// Create the index buffer.
	device->CreateBuffer(&indexBufferDesc, &indexData, &indexBuffer);

	// Release the arrays now that the buffers have been created and loaded.
	delete[] vertices;
	vertices = 0;
	delete[] indices;
	indices = 0;
}