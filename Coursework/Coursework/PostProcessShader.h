#pragma once

#include "DXF.h"

using namespace std;
using namespace DirectX;

class PostProcessShader : public BaseShader
{
public:
	PostProcessShader(ID3D11Device* device, HWND hwnd);
	~PostProcessShader();

	// Buffer type for screen information and DoF data
	struct ScreenBufferType
	{
		float screenWidth;
		float screenHeight;
		float tolerance;
		float on;
	};

	void setDepthOfFieldShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* depthMap, float screenWidth, float screenHeight, float DoFTolerance, float DoFToggle);

private:
	void initShader(const wchar_t* cs, const wchar_t* ps);

private:
	ID3D11Buffer* matrixBuffer;
	ID3D11Buffer* screenBuffer;
	ID3D11SamplerState* sampleState;
};

