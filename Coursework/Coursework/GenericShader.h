#pragma once

#include "DXF.h"

using namespace std;
using namespace DirectX;

class GenericShader : public BaseShader
{

private:
	// Buffer type for a directional light
	struct DirLightBufferType
	{
		XMFLOAT4 diffuse;
		XMFLOAT3 direction;
		float padding = 0.0f;
	};

	// Buffer type for a point light
	struct PLightBufferType
	{
		XMFLOAT4 diffuse;
		XMFLOAT3 position;
		float specular;
	};

	// Buffer type for a spotlight
	struct SpotLightBufferType
	{
		XMFLOAT4 diffuse;
		XMFLOAT3 direction;
		float cutoff;
		XMFLOAT3 position;
		float padding = 0.0f;
	};

	// Buffer type for camera data
	struct CameraBufferType
	{
		XMFLOAT3 position;
		float padding = 0.0f;
	};

	// Redefiniton of buffer type for matrices
	struct MatrixBufferType
	{
		XMMATRIX worldMatrix;
		XMMATRIX viewMatrix;
		XMMATRIX projectionMatrix;

		XMMATRIX lightViewMatrix1;
		XMMATRIX lightProjectionMatrix1;

		XMMATRIX lightViewMatrix2;
		XMMATRIX lightProjectionMatrix2;
	};

public:
	GenericShader(ID3D11Device* device, HWND hwnd);
	~GenericShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* depthMap, ID3D11ShaderResourceView* depthMap2, Light* dirLight, Light* pLight1, float pRange1, Light* pLight2, float pRange2, Light* spotLight, float spotCutoff, Camera* camera);

private:
	void initShader(const wchar_t* cs, const wchar_t* ps);

private:
	ID3D11Buffer* matrixBuffer;
	ID3D11SamplerState* sampleState;
	ID3D11SamplerState* sampleStateShadow;
	ID3D11Buffer* dirLightBuffer;
	ID3D11Buffer* pLight1Buffer;
	ID3D11Buffer* pLight2Buffer;
	ID3D11Buffer* spotLightBuffer;
	ID3D11Buffer* camBuffer;
};

