#include "TessellationDepthShader.h"

TessellationDepthShader::TessellationDepthShader(ID3D11Device* device, HWND hwnd) : BaseShader(device, hwnd)
{
	initShader(L"TessellationVS.cso", L"TessellationHS.cso", L"HeightMapTessellationDepthDS.cso", L"DepthPS.cso");
}

TessellationDepthShader::~TessellationDepthShader()
{
	// Release the matrix constant buffer.
	if (matrixBuffer)
	{
		matrixBuffer->Release();
		matrixBuffer = 0;
	}
	
	// Release the matrix constant buffer.
	if (paramBuffer)
	{
		paramBuffer->Release();
		paramBuffer = 0;
	}
	
	// Release the matrix constant buffer.
	if (sampleState)
	{
		sampleState->Release();
		sampleState = 0;
	}

	// Release the layout.
	if (layout)
	{
		layout->Release();
		layout = 0;
	}

	//Release base shader components
	BaseShader::~BaseShader();
}

void TessellationDepthShader::initShader(const wchar_t* vsFilename, const wchar_t* psFilename)
{
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC waveBufferDesc;

	// Load (+ compile) shader files
	loadVertexShader(vsFilename);
	loadPixelShader(psFilename);

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&matrixBufferDesc, NULL, &matrixBuffer);

	// Create a parameter buffer for tesselation
	D3D11_BUFFER_DESC paramBufferDesc;
	paramBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	paramBufferDesc.ByteWidth = sizeof(ParamBufferType);
	paramBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	paramBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	paramBufferDesc.MiscFlags = 0;
	paramBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&paramBufferDesc, NULL, &paramBuffer);

	// Create a texture sampler state description.
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	renderer->CreateSamplerState(&samplerDesc, &sampleState);
}

void TessellationDepthShader::initShader(const wchar_t* vsFilename, const wchar_t* hsFilename, const wchar_t* dsFilename, const wchar_t* psFilename)
{
	// InitShader must be overwritten and it will load both vertex and pixel shaders + setup buffers
	initShader(vsFilename, psFilename);

	// Load other required shaders.
	loadHullShader(hsFilename);
	loadDomainShader(dsFilename);
}

void TessellationDepthShader::setHeightMapShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, float resolution, float factor, ID3D11ShaderResourceView* heightMap)
{
	loadDomainShader(L"HeightMapTessellationDepthDS.cso");

	HRESULT result;

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;

	// Transpose the matrices to prepare them for the shader.
	XMMATRIX tworld = XMMatrixTranspose(worldMatrix);
	XMMATRIX tview = XMMatrixTranspose(viewMatrix);
	XMMATRIX tproj = XMMatrixTranspose(projectionMatrix);

	// Lock the constant buffer so it can be written to.
	deviceContext->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	dataPtr = (MatrixBufferType*)mappedResource.pData;
	dataPtr->world = tworld;// worldMatrix;
	dataPtr->view = tview;
	dataPtr->projection = tproj;
	deviceContext->Unmap(matrixBuffer, 0);
	deviceContext->DSSetConstantBuffers(0, 1, &matrixBuffer);
	deviceContext->DSSetShaderResources(0, 1, &heightMap);
	deviceContext->DSSetSamplers(0, 1, &sampleState);

	// Set variables passed into the param buffer
	result = deviceContext->Map(paramBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	ParamBufferType* paramPtr = (ParamBufferType*)mappedResource.pData;
	paramPtr->tessFactor = factor;
	paramPtr->planeRes = resolution;
	deviceContext->Unmap(paramBuffer, 0);
	deviceContext->HSSetConstantBuffers(0, 1, &paramBuffer);
	deviceContext->DSSetConstantBuffers(1, 1, &paramBuffer);
}