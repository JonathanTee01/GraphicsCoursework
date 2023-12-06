#include "HeightMapShader.h"

HeightMapShader::HeightMapShader(ID3D11Device* device, HWND hwnd) : BaseShader(device, hwnd)
{
	initShader(L"HeightMapVS.cso", L"NoReflectPS.cso");
}


HeightMapShader::~HeightMapShader()
{
	// Release the matrix constant buffer.
	if (matrixBuffer)
	{
		matrixBuffer->Release();
		matrixBuffer = 0;
	}

	// Release the sampler state.
	if (sampleState)
	{
		sampleState->Release();
		sampleState = 0;
	}

	// Release the sampler state.
	if (sampleStateShadow)
	{
		sampleStateShadow->Release();
		sampleStateShadow = 0;
	}

	// Release the layout.
	if (layout)
	{
		layout->Release();
		layout = 0;
	}

	// Release the light constant buffer.
	if (dirLightBuffer)
	{
		dirLightBuffer->Release();
		dirLightBuffer = 0;
	}

	// Release the light constant buffer.
	if (pLight1Buffer)
	{
		pLight1Buffer->Release();
		pLight1Buffer = 0;
	}

	// Release the light constant buffer.
	if (pLight2Buffer)
	{
		pLight2Buffer->Release();
		pLight2Buffer = 0;
	}

	// Release the light constant buffer.
	if (spotLightBuffer)
	{
		spotLightBuffer->Release();
		spotLightBuffer = 0;
	}

	// Release the camera constant buffer.
	if (camBuffer)
	{
		camBuffer->Release();
		camBuffer = 0;
	}

	//Release base shader components
	BaseShader::~BaseShader();
}

void HeightMapShader::initShader(const wchar_t* vsFilename, const wchar_t* psFilename)
{
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC lightBufferDesc;

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

	// Setup light buffer
	// Setup the description of the light dynamic constant buffer that is in the pixel shader.
	// Note that ByteWidth always needs to be a multiple of 16 if using D3D11_BIND_CONSTANT_BUFFER or CreateBuffer will fail.
    // Directional
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(DirLightBufferType);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&lightBufferDesc, NULL, &dirLightBuffer);

    // Point light 1
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(PLightBufferType);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&lightBufferDesc, NULL, &pLight1Buffer);

    // Point light 2
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(PLightBufferType);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&lightBufferDesc, NULL, &pLight2Buffer);

    // Spotlight
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(SpotLightBufferType);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&lightBufferDesc, NULL, &spotLightBuffer);

	// Sampler for shadow map sampling.
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.BorderColor[0] = 1.0f;
	samplerDesc.BorderColor[1] = 1.0f;
	samplerDesc.BorderColor[2] = 1.0f;
	samplerDesc.BorderColor[3] = 1.0f;
	renderer->CreateSamplerState(&samplerDesc, &sampleStateShadow);

	// Buffer for the cameras position
	// Used for specular lighting
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(CameraBufferType);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&lightBufferDesc, NULL, &camBuffer);
}

void HeightMapShader::setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* depthMap, ID3D11ShaderResourceView* depthMap2, Light* dirLight, Light* pLight1, float pSpecular1, Light* pLight2, float pSpecular2, Light* spotLight, float spotCutoff, Camera* camera, ID3D11ShaderResourceView* heightMap)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;

	XMMATRIX tworld, tview, tproj;

	// Transpose the matrices to prepare them for the shader.
	tworld = XMMatrixTranspose(worldMatrix);
	tview = XMMatrixTranspose(viewMatrix);
	tproj = XMMatrixTranspose(projectionMatrix);
	result = deviceContext->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	dataPtr = (MatrixBufferType*)mappedResource.pData;
	dataPtr->worldMatrix = tworld;// worldMatrix;
	dataPtr->viewMatrix = tview;
	dataPtr->projectionMatrix = tproj;
	dataPtr->lightViewMatrix1 = XMMatrixTranspose(dirLight->getViewMatrix());
	dataPtr->lightProjectionMatrix1 = XMMatrixTranspose(dirLight->getOrthoMatrix());
	dataPtr->lightViewMatrix2 = XMMatrixTranspose(spotLight->getViewMatrix());
	dataPtr->lightProjectionMatrix2 = XMMatrixTranspose(spotLight->getProjectionMatrix());
	deviceContext->Unmap(matrixBuffer, 0);
	deviceContext->VSSetConstantBuffers(0, 1, &matrixBuffer);
	deviceContext->VSSetShaderResources(0, 1, &heightMap);
	deviceContext->VSSetSamplers(0, 1, &sampleState);


	// Send light data to pixel shader for directional light
	DirLightBufferType* dirLightPtr;
	deviceContext->Map(dirLightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	dirLightPtr = (DirLightBufferType*)mappedResource.pData;
	dirLightPtr->diffuse = dirLight->getDiffuseColour();
	dirLightPtr->direction = dirLight->getDirection();
	deviceContext->Unmap(dirLightBuffer, 0);
	deviceContext->PSSetConstantBuffers(0, 1, &dirLightBuffer);


	// Send light data to pixel shader for point lights
   // Point light 1
	PLightBufferType* p1LightPtr;
	deviceContext->Map(pLight1Buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	p1LightPtr = (PLightBufferType*)mappedResource.pData;
	p1LightPtr->diffuse = pLight1->getDiffuseColour();
	p1LightPtr->position = pLight1->getPosition();
	p1LightPtr->specular = pSpecular1;

	deviceContext->Unmap(pLight1Buffer, 0);
	deviceContext->PSSetConstantBuffers(1, 1, &pLight1Buffer);

   // Point light 2
	PLightBufferType* p2LightPtr;
	deviceContext->Map(pLight2Buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	p2LightPtr = (PLightBufferType*)mappedResource.pData;
	p2LightPtr->diffuse = pLight2->getDiffuseColour();
	p2LightPtr->position = pLight2->getPosition();
	p2LightPtr->specular = pSpecular2;

	deviceContext->Unmap(pLight2Buffer, 0);
	deviceContext->PSSetConstantBuffers(2, 1, &pLight2Buffer);

	// Send light data to pixel shader for spot light
	SpotLightBufferType* spotLightPtr;
	deviceContext->Map(spotLightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	spotLightPtr = (SpotLightBufferType*)mappedResource.pData;
	spotLightPtr->diffuse = spotLight->getDiffuseColour();
	spotLightPtr->direction = spotLight->getDirection();
	spotLightPtr->position = spotLight->getPosition();
	spotLightPtr->cutoff = spotCutoff;

	deviceContext->Unmap(spotLightBuffer, 0);
	deviceContext->PSSetConstantBuffers(3, 1, &spotLightBuffer);

	// Camera buffer
	CameraBufferType* camPtr;
	deviceContext->Map(camBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	camPtr = (CameraBufferType*)mappedResource.pData;
	camPtr->position = camera->getPosition();

	deviceContext->Unmap(camBuffer, 0);
	deviceContext->PSSetConstantBuffers(4, 1, &camBuffer);
	

	// Set shader texture resource in the pixel shader.
	deviceContext->PSSetShaderResources(0, 1, &texture);
	deviceContext->PSSetShaderResources(1, 1, &depthMap);
	deviceContext->PSSetShaderResources(2, 1, &depthMap2);
	deviceContext->PSSetSamplers(0, 1, &sampleState);
	deviceContext->PSSetSamplers(1, 1, &sampleStateShadow);

}