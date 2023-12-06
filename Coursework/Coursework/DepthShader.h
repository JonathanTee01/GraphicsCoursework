// Colour shader.h
// Simple shader example.
#pragma once

#include "DXF.h"

using namespace std;
using namespace DirectX;


class DepthShader : public BaseShader
{
private:
	// Buffer type for wave data
	struct WaveBufferType
	{
		float time;
		float amplitude;
		float frequency;
		float speed;
	};

public:
	DepthShader(ID3D11Device* device, HWND hwnd);
	~DepthShader();

	void setHeightMapShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, ID3D11ShaderResourceView* heightMap);
	void setWaveShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, float time, float frequency, float height);
	void setStandardShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection);

private:
	void initShader(const wchar_t* vs, const wchar_t* ps);

private:
	ID3D11Buffer* matrixBuffer;
	ID3D11Buffer* waveBuffer;
	ID3D11SamplerState* sampleState;
};
