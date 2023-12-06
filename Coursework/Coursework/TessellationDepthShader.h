// Colour shader.h
// Simple shader example.
#pragma once

#include "DXF.h"

using namespace std;
using namespace DirectX;


class TessellationDepthShader : public BaseShader
{
private:
	// Buffer type for tesselation variables
	struct ParamBufferType
	{
		float tessFactor;
		float planeRes;
		XMFLOAT2 padding = { 0.0f, 0.0f };
	};

	// Buffer type for wave data
	struct WaveBufferType
	{
		float time;
		float amplitude;
		float frequency;
		float speed;
	};

public:
	TessellationDepthShader(ID3D11Device* device, HWND hwnd);
	~TessellationDepthShader();

	void setHeightMapShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, float resolution, float factor, ID3D11ShaderResourceView* heightMap);
private:
	void initShader(const wchar_t* vs, const wchar_t* ps);
	void initShader(const wchar_t* vs, const wchar_t* hs, const wchar_t* ds, const wchar_t* ps);

private:
	ID3D11Buffer* matrixBuffer;
	ID3D11Buffer* paramBuffer;
	ID3D11SamplerState* sampleState;
};