// Lab1.cpp
// Lab 1 example, simple coloured triangle mesh
#include "App1.h"

App1::App1()
{
	heightMesh = nullptr;
	waveMesh = nullptr;
	heightShader = nullptr;
	waveShader = nullptr;
}

void App1::init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input* in, bool VSYNC, bool FULL_SCREEN)
{
	// Call super/parent init function (required!)
	BaseApplication::init(hinstance, hwnd, screenWidth, screenHeight, in, VSYNC, FULL_SCREEN);

	// Store screen variables
	width = screenWidth;
	height = screenHeight;

	// Load texture.
	textureMgr->loadTexture(L"wave", L"res/Water.png");
	textureMgr->loadTexture(L"hill", L"res/Hill.png");
	textureMgr->loadTexture(L"height", L"res/heightmap.png");

	// Create Mesh object and shader object
	heightMesh = new PlaneMesh(renderer->getDevice(), renderer->getDeviceContext());
	tessHeightMesh = new TessellationPlane(renderer->getDevice(), renderer->getDeviceContext(), 100);
	waveMesh = new PlaneMesh(renderer->getDevice(), renderer->getDeviceContext());
	tessWaveMesh = new TessellationPlane(renderer->getDevice(), renderer->getDeviceContext(), 100);
	hut = new AModel(renderer->getDevice(), "res/changing_room.obj");
	ortho = new OrthoMesh(renderer->getDevice(), renderer->getDeviceContext(), screenWidth, screenHeight, 0, 0);
	screenTexture = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);

	// Initialise Shaders
	heightShader = new HeightMapShader(renderer->getDevice(), hwnd);
	waveShader = new WaveShader(renderer->getDevice(), hwnd);
	genericShader = new GenericShader(renderer->getDevice(), hwnd);
	depthShader = new DepthShader(renderer->getDevice(), hwnd);
	postProcessShader = new PostProcessShader(renderer->getDevice(), hwnd);
	tessShader = new TessellationShader(renderer->getDevice(), hwnd);
	tessDepthShader = new TessellationDepthShader(renderer->getDevice(), hwnd);

	// Shadowmaps
	shadowMap = new ShadowMap(renderer->getDevice(), 2048 * 4, 2048 * 4);
	shadowMap2 = new ShadowMap(renderer->getDevice(), 2048 * 4, 2048 * 4);

	// initialise depth map for camera
	depthMap = new ShadowMap(renderer->getDevice(), 2048 * 4, 2048 * 4);

	// Confirgure directional light
	dirLight = new Light();
	dirLight->setDirection(dirDirection[0], dirDirection[1], dirDirection[2]);
	dirLight->setDiffuseColour(dirColour[0], dirColour[1], dirColour[2], 0.7f);
	dirLight->generateOrthoMatrix(141, 141, 0.1f, 150.f);

	// Configure point lights
	pLight1 = new Light();
	lightMesh1 = new SphereMesh(renderer->getDevice(), renderer->getDeviceContext());
	pLight1->setPosition(p1Pos[0], p1Pos[1], p1Pos[2]);
	pLight1->setDiffuseColour(p1Rgb[0], p1Rgb[1], p1Rgb[2], 0.7f);

	pLight2 = new Light();
	lightMesh2 = new SphereMesh(renderer->getDevice(), renderer->getDeviceContext());
	pLight2->setPosition(p2Pos[0], p2Pos[1], p2Pos[2]);
	pLight2->setDiffuseColour(p2Rgb[0], p2Rgb[1], p2Rgb[2], 0.7f);

	// Configure spot light
	spotLight = new Light();
	lightMesh3 = new SphereMesh(renderer->getDevice(), renderer->getDeviceContext());
	spotLight->setDirection(spotDirection[0], spotDirection[1], spotDirection[2]);
	spotLight->setPosition(spotPosition[0], spotPosition[1], spotPosition[2]);
	spotLight->setDiffuseColour(spotColour[0], spotColour[1], spotColour[2], 0.7f);
	spotLight->generateProjectionMatrix(0.1f, 150.0f);
}

App1::~App1()
{
	// Run base application deconstructor
	BaseApplication::~BaseApplication();

	// Release the Direct3D object.
	if (genericShader) 
	{
		delete genericShader;
		genericShader = 0;
	}

	if (tessShader)
	{
		delete tessShader;
		tessShader = 0;
	}

	if (tessDepthShader)
	{
		delete tessDepthShader;
		tessDepthShader = 0;
	}

	if (heightShader)
	{
		delete heightShader;
		heightShader = 0;
	}

	if (tessHeightMesh)
	{
		delete tessHeightMesh;
		tessHeightMesh = 0;
	}

	if (heightMesh)
	{
		delete heightMesh;
		heightMesh = 0;
	}

	if (hut)
	{
		delete hut;
		hut = 0;
	}

	if (waveShader)
	{
		delete waveShader;
		waveShader = 0;
	}

	if (waveMesh)
	{
		delete waveMesh;
		waveMesh = 0;
	}

	if (lightMesh1)
	{
		delete lightMesh1;
		lightMesh1 = 0;
	}

	if (lightMesh2)
	{
		delete lightMesh2;
		lightMesh2 = 0;
	}

	if (lightMesh2)
	{
		delete lightMesh2;
		lightMesh2 = 0;
	}

	if (depthShader)
	{
		delete lightMesh1;
		lightMesh1 = 0;
	}

	if (ortho)
	{
		delete ortho;
		ortho = 0;
	}

	if (postProcessShader)
	{
		delete postProcessShader;
		postProcessShader = 0;
	}
}

bool App1::frame()
{
	bool result;

	// Update time.
	time += 0.003f;

	result = BaseApplication::frame();
	if (!result)
	{
		return false;
	}

	// Render the graphics.
	result = render();
	if (!result)
	{
		return false;
	}

	return true;
}

bool App1::render() 
{
	// Perform shadow passes
	shadowPass();
	shadowPass2();

	// Perform the depth pass
	depthPass();

	// Perform the final pass
	finalPass();

	// Perform the post pass
	if (DoFToggle) 
	{
		// If depth of field is on then do the post process
		postPass();
	}

	// Toggle for depth of field
	// Toggles here because the gui updates mid-frame and the code isn't happy with that
	if (DoFSwitch) 
	{
		DoFToggle = true;
	}
	else 
	{
		DoFToggle = false;
	}

	return true;
}

void App1::shadowPass()
{
	// Set the render target to be the render to texture.
	shadowMap->BindDsvAndSetNullRenderTarget(renderer->getDeviceContext());

	// Move the directional light to an appropriate position
	XMFLOAT3 lightOffset = dirLight->getDirection();
	lightOffset = XMFLOAT3(-lightOffset.x * 71, -lightOffset.y * 71, -lightOffset.z * 71);
	dirLight->setPosition(lightOffset.x + 50.0f, lightOffset.y, lightOffset.z + 50.0f);

	// get the world, view, and projection matrices from the light and d3d objects.
	dirLight->generateViewMatrix();
	XMMATRIX lightViewMatrix = dirLight->getViewMatrix();
	XMMATRIX lightProjectionMatrix = dirLight->getOrthoMatrix();
	XMMATRIX worldMatrix = renderer->getWorldMatrix();

	// Render height map
	// Tesselated for accurate shadow
	tessHeightMesh->sendData(renderer->getDeviceContext(), D3D_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);
	tessDepthShader->setHeightMapShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix, 100, heightMapTess, textureMgr->getTexture(L"height"));
	tessDepthShader->render(renderer->getDeviceContext(), tessHeightMesh->getIndexCount());

	// Waves
	// Not tessellated to save resources
	worldMatrix = XMMatrixTranslation(0.0f, 2.0f, 0.0f);
	waveMesh->sendData(renderer->getDeviceContext());
	depthShader->setWaveShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix, time, waveFrequency, waveHeight);
	depthShader->render(renderer->getDeviceContext(), waveMesh->getIndexCount());

	// Huts
   // Hut 1
	worldMatrix = XMMatrixTranslation(32.0f, 3.2f, 44.0f);
	hut->sendData(renderer->getDeviceContext());
	depthShader->setStandardShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
	depthShader->render(renderer->getDeviceContext(), hut->getIndexCount());

   // Hut 2
	worldMatrix = XMMatrixTranslation(64.0f, 3.7f, 65.0f);
	hut->sendData(renderer->getDeviceContext());
	depthShader->setStandardShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
	depthShader->render(renderer->getDeviceContext(), hut->getIndexCount());

   // Hut 3
	worldMatrix = XMMatrixTranslation(41.0f, 12.0f, 64.5f);
	hut->sendData(renderer->getDeviceContext());
	depthShader->setStandardShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
	depthShader->render(renderer->getDeviceContext(), hut->getIndexCount());

	// Light meshes
   // Point light 1
	worldMatrix = XMMatrixTranslation(pLight1->getPosition().x, pLight1->getPosition().y, pLight1->getPosition().z);
	lightMesh1->sendData(renderer->getDeviceContext());
	depthShader->setStandardShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
	depthShader->render(renderer->getDeviceContext(), lightMesh1->getIndexCount());

   // Point light 2
	worldMatrix = XMMatrixTranslation(pLight2->getPosition().x, pLight2->getPosition().y, pLight2->getPosition().z);
	lightMesh2->sendData(renderer->getDeviceContext());
	depthShader->setStandardShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
	depthShader->render(renderer->getDeviceContext(), lightMesh2->getIndexCount());

   // Spotlight
	worldMatrix = XMMatrixTranslation(spotLight->getPosition().x, spotLight->getPosition().y, spotLight->getPosition().z);
	lightMesh3->sendData(renderer->getDeviceContext());
	depthShader->setStandardShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
	depthShader->render(renderer->getDeviceContext(), lightMesh3->getIndexCount());


	// Set back buffer as render target and reset view port.
	renderer->setBackBufferRenderTarget();
	renderer->resetViewport();
}

void App1::shadowPass2()
{
	// Set the render target to be the render to texture.
	shadowMap2->BindDsvAndSetNullRenderTarget(renderer->getDeviceContext());

	// get the world, view, and projection matrices from the light and d3d objects.
	spotLight->generateViewMatrix();
	XMMATRIX lightViewMatrix = spotLight->getViewMatrix();
	XMMATRIX lightProjectionMatrix = spotLight->getProjectionMatrix();
	XMMATRIX worldMatrix = renderer->getWorldMatrix();

	// Render height map
	// Tesselated for accurate shadow
	tessHeightMesh->sendData(renderer->getDeviceContext(), D3D_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);
	tessDepthShader->setHeightMapShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix, 100, heightMapTess, textureMgr->getTexture(L"height"));
	tessDepthShader->render(renderer->getDeviceContext(), tessHeightMesh->getIndexCount());

	// Waves
	// Not tessellated to save resources
	worldMatrix = XMMatrixTranslation(0.0f, 2.0f, 0.0f);
	waveMesh->sendData(renderer->getDeviceContext());
	depthShader->setWaveShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix, time, waveFrequency, waveHeight);
	depthShader->render(renderer->getDeviceContext(), waveMesh->getIndexCount());

	// Huts
   // Hut 1
	worldMatrix = XMMatrixTranslation(32.0f, 3.2f, 44.0f);
	hut->sendData(renderer->getDeviceContext());
	depthShader->setStandardShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
	depthShader->render(renderer->getDeviceContext(), hut->getIndexCount());

   // Hut 2
	worldMatrix = XMMatrixTranslation(64.0f, 3.7f, 65.0f);
	hut->sendData(renderer->getDeviceContext());
	depthShader->setStandardShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
	depthShader->render(renderer->getDeviceContext(), hut->getIndexCount());

   // Hut 3
	worldMatrix = XMMatrixTranslation(41.0f, 12.0f, 64.5f);
	hut->sendData(renderer->getDeviceContext());
	depthShader->setStandardShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
	depthShader->render(renderer->getDeviceContext(), hut->getIndexCount());

	// Light meshes
   // Point light 1
	worldMatrix = XMMatrixTranslation(pLight1->getPosition().x, pLight1->getPosition().y, pLight1->getPosition().z);
	lightMesh1->sendData(renderer->getDeviceContext());
	depthShader->setStandardShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
	depthShader->render(renderer->getDeviceContext(), lightMesh1->getIndexCount());

   // Point light 2
	worldMatrix = XMMatrixTranslation(pLight2->getPosition().x, pLight2->getPosition().y, pLight2->getPosition().z);
	lightMesh2->sendData(renderer->getDeviceContext());
	depthShader->setStandardShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
	depthShader->render(renderer->getDeviceContext(), lightMesh2->getIndexCount());

   // Spotlight
	worldMatrix = XMMatrixTranslation(spotLight->getPosition().x, spotLight->getPosition().y, spotLight->getPosition().z);
	lightMesh3->sendData(renderer->getDeviceContext());
	depthShader->setStandardShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
	depthShader->render(renderer->getDeviceContext(), lightMesh3->getIndexCount());


	// Set back buffer as render target and reset view port.
	renderer->setBackBufferRenderTarget();
	renderer->resetViewport();
}

void App1::depthPass()
{
	// Set the render target to be the render to texture.
	depthMap->BindDsvAndSetNullRenderTarget(renderer->getDeviceContext());

	// get the world, view, and projection matrices from the light and d3d objects.
	XMMATRIX cameraViewMatrix = camera->getViewMatrix();
	XMMATRIX cameraProjectionMatrix = renderer->getProjectionMatrix();
	XMMATRIX worldMatrix = renderer->getWorldMatrix();

	// Render height map
	// Not tessellated due to depth sampling being similar depths whether tessellated or not
	heightMesh->sendData(renderer->getDeviceContext());
	depthShader->setHeightMapShaderParameters(renderer->getDeviceContext(), worldMatrix, cameraViewMatrix, cameraProjectionMatrix, textureMgr->getTexture(L"height"));
	depthShader->render(renderer->getDeviceContext(), heightMesh->getIndexCount());

	// Waves
	// Not tesselated as both meshes are going to be similar due to mathematical nature so we can save on resources
	worldMatrix = XMMatrixTranslation(0.0f, 2.0f, 0.0f);
	waveMesh->sendData(renderer->getDeviceContext());
	depthShader->setWaveShaderParameters(renderer->getDeviceContext(), worldMatrix, cameraViewMatrix, cameraProjectionMatrix, time, waveFrequency, waveHeight);
	depthShader->render(renderer->getDeviceContext(), waveMesh->getIndexCount());

	// Huts
   // Hut 1
	worldMatrix = XMMatrixTranslation(32.0f, 3.2f, 44.0f);
	hut->sendData(renderer->getDeviceContext());
	depthShader->setStandardShaderParameters(renderer->getDeviceContext(), worldMatrix, cameraViewMatrix, cameraProjectionMatrix);
	depthShader->render(renderer->getDeviceContext(), hut->getIndexCount());

   // Hut 2
	worldMatrix = XMMatrixTranslation(64.0f, 3.7f, 65.0f);
	hut->sendData(renderer->getDeviceContext());
	depthShader->setStandardShaderParameters(renderer->getDeviceContext(), worldMatrix, cameraViewMatrix, cameraProjectionMatrix);
	depthShader->render(renderer->getDeviceContext(), hut->getIndexCount());

   // Hut 3
	worldMatrix = XMMatrixTranslation(41.0f, 12.0f, 64.5f);
	hut->sendData(renderer->getDeviceContext());
	depthShader->setStandardShaderParameters(renderer->getDeviceContext(), worldMatrix, cameraViewMatrix, cameraProjectionMatrix);
	depthShader->render(renderer->getDeviceContext(), hut->getIndexCount());

	// Light meshes
   // Point light 1
	worldMatrix = XMMatrixTranslation(pLight1->getPosition().x, pLight1->getPosition().y, pLight1->getPosition().z);
	lightMesh1->sendData(renderer->getDeviceContext());
	depthShader->setStandardShaderParameters(renderer->getDeviceContext(), worldMatrix, cameraViewMatrix, cameraProjectionMatrix);
	depthShader->render(renderer->getDeviceContext(), lightMesh1->getIndexCount());

   // Point light 2
	worldMatrix = XMMatrixTranslation(pLight2->getPosition().x, pLight2->getPosition().y, pLight2->getPosition().z);
	lightMesh2->sendData(renderer->getDeviceContext());
	depthShader->setStandardShaderParameters(renderer->getDeviceContext(), worldMatrix, cameraViewMatrix, cameraProjectionMatrix);
	depthShader->render(renderer->getDeviceContext(), lightMesh2->getIndexCount());

   // Spotlight
	worldMatrix = XMMatrixTranslation(spotLight->getPosition().x, spotLight->getPosition().y, spotLight->getPosition().z);
	lightMesh3->sendData(renderer->getDeviceContext());
	depthShader->setStandardShaderParameters(renderer->getDeviceContext(), worldMatrix, cameraViewMatrix, cameraProjectionMatrix);
	depthShader->render(renderer->getDeviceContext(), lightMesh3->getIndexCount());


	// Set back buffer as render target and reset view port.
	renderer->setBackBufferRenderTarget();
	renderer->resetViewport();
}

// Despite it's name it isn't the final pass if a post process is turned on
void App1::finalPass()
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix; 

	// If depth of field is on we want to render teh scene to a texture
	if (DoFToggle)
	{
		screenTexture->setRenderTarget(renderer->getDeviceContext());
		screenTexture->clearRenderTarget(renderer->getDeviceContext(), 0.39f, 0.58f, 0.92f, 1.0f);
	}
	// Otherwise treat it as normal
	else {
		renderer->beginScene(0.39f, 0.58f, 0.92f, 1.0f);
	}

	// Generate the view matrix based on the camera's position.
	camera->update();

	// Get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
	worldMatrix = renderer->getWorldMatrix();
	viewMatrix = camera->getViewMatrix();
	projectionMatrix = renderer->getProjectionMatrix();

	// Send geometry data, set shader parameters, render object with shader
	tessHeightMesh->sendData(renderer->getDeviceContext(), D3D_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);
	tessShader->setHeightMapShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, 100, heightMapTess, textureMgr->getTexture(L"hill"), shadowMap->getDepthMapSRV(), shadowMap2->getDepthMapSRV(), dirLight, pLight1, p1Specular, pLight2, p2Specular, spotLight, cutoff, camera, textureMgr->getTexture(L"height"));
	tessShader->render(renderer->getDeviceContext(), tessHeightMesh->getIndexCount());

	// Waves
	worldMatrix = XMMatrixTranslation(0.0f, 2.0f, 0.0f);
	tessWaveMesh->sendData(renderer->getDeviceContext(), D3D_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);
	tessShader->setWaveShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, 100, waveTess, textureMgr->getTexture(L"wave"), shadowMap->getDepthMapSRV(), shadowMap2->getDepthMapSRV(), dirLight, pLight1, p1Specular, pLight2, p2Specular, spotLight, cutoff, camera, time, waveFrequency, waveHeight);
	tessShader->render(renderer->getDeviceContext(), tessWaveMesh->getIndexCount());

	// Huts
   // Hut 1
	worldMatrix = XMMatrixTranslation(32.0f, 3.2f, 44.0f);
	hut->sendData(renderer->getDeviceContext());
	genericShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"blank"), shadowMap->getDepthMapSRV(), shadowMap2->getDepthMapSRV(), dirLight, pLight1, p1Specular, pLight2, p2Specular, spotLight, cutoff, camera);
	genericShader->render(renderer->getDeviceContext(), hut->getIndexCount());

   // Hut 2
	worldMatrix = XMMatrixTranslation(64.0f, 3.7f, 65.0f);
	hut->sendData(renderer->getDeviceContext());
	genericShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"blank"), shadowMap->getDepthMapSRV(), shadowMap2->getDepthMapSRV(), dirLight, pLight1, p1Specular, pLight2, p2Specular, spotLight, cutoff, camera);
	genericShader->render(renderer->getDeviceContext(), hut->getIndexCount());

   // Hut 3
	worldMatrix = XMMatrixTranslation(41.0f, 12.0f, 64.5f);
	hut->sendData(renderer->getDeviceContext());
	genericShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"blank"), shadowMap->getDepthMapSRV(), shadowMap2->getDepthMapSRV(), dirLight, pLight1, p1Specular, pLight2, p2Specular, spotLight, cutoff, camera);
	genericShader->render(renderer->getDeviceContext(), hut->getIndexCount());

	// Light meshes
   // Point light 1
	worldMatrix = XMMatrixTranslation(pLight1->getPosition().x, pLight1->getPosition().y, pLight1->getPosition().z);
	lightMesh1->sendData(renderer->getDeviceContext());
	genericShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"blank"), shadowMap->getDepthMapSRV(), shadowMap2->getDepthMapSRV(), dirLight, pLight1, p1Specular, pLight2, p2Specular, spotLight, cutoff, camera);
	genericShader->render(renderer->getDeviceContext(), lightMesh1->getIndexCount());

   // Point light 2
	worldMatrix = XMMatrixTranslation(pLight2->getPosition().x, pLight2->getPosition().y, pLight2->getPosition().z);
	lightMesh2->sendData(renderer->getDeviceContext());
	genericShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"blank"), shadowMap->getDepthMapSRV(), shadowMap2->getDepthMapSRV(), dirLight, pLight1, p1Specular, pLight2, p2Specular, spotLight, cutoff, camera);
	genericShader->render(renderer->getDeviceContext(), lightMesh2->getIndexCount());

   // Spotlight
	worldMatrix = XMMatrixTranslation(spotLight->getPosition().x, spotLight->getPosition().y, spotLight->getPosition().z);
	lightMesh3->sendData(renderer->getDeviceContext());
	genericShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"blank"), shadowMap->getDepthMapSRV(), shadowMap2->getDepthMapSRV(), dirLight, pLight1, p1Specular, pLight2, p2Specular, spotLight, cutoff, camera);
	genericShader->render(renderer->getDeviceContext(), lightMesh3->getIndexCount());

	// If depth of field is on then reset the render target
	if (DoFToggle)
	{
		renderer->setBackBufferRenderTarget();
		renderer->resetViewport();
	}
	// Otherwise this is the final scene so render the gui and the send the scene to the screen
	else 
	{
		// Render GUI
		gui();

		// Reset the render target back to the original back buffer and not the render to texture anymore.
		renderer->endScene();
	}
}

void App1::postPass() 
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;

	// Clear the scene. (default blue colour)
	renderer->beginScene(0.39f, 0.58f, 0.92f, 1.0f);

	worldMatrix = renderer->getWorldMatrix();
	viewMatrix = camera->getViewMatrix();
	projectionMatrix = renderer->getProjectionMatrix();

	// Disable z buffer
	renderer->setZBuffer(false);
	worldMatrix = renderer->getWorldMatrix();
	XMMATRIX orthoMatrix = renderer->getOrthoMatrix();  
	XMMATRIX orthoViewMatrix = camera->getOrthoViewMatrix();	

	// Render an ortho to the screen with the screen texture and post process
	ortho->sendData(renderer->getDeviceContext());
	postProcessShader->setDepthOfFieldShaderParameters(renderer->getDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, screenTexture->getShaderResourceView(), depthMap->getDepthMapSRV(), width, height, DoFTolerance, float(DoFToggle));
	postProcessShader->render(renderer->getDeviceContext(), ortho->getIndexCount());

	// Reenable z buffer
	renderer->setZBuffer(true);

	// Render GUI
	gui();

	// Send to screen
	renderer->endScene();
}

void App1::gui()
{
	// Force turn off unnecessary shader stages.
	renderer->getDeviceContext()->GSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->HSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->DSSetShader(NULL, NULL, 0);

	// Build UI
	ImGui::Text("FPS: %.2f", timer->getFPS());
	ImGui::Checkbox("Wireframe mode", &wireframeToggle);

	// Collapsing header for waves
	if (ImGui::CollapsingHeader("Waves")) 
	{
		ImGui::DragFloat("Wave frequency", &waveFrequency, 0.1f);
		ImGui::DragFloat("Wave height", &waveHeight, 0.1f);
	}

	// Collapsing header for lights
	if (ImGui::CollapsingHeader("Lights"))
	{
		// Direcrtional
		if (ImGui::CollapsingHeader("Directional Light"))
		{
			ImGui::DragFloat3("Direction", dirDirection, 0.01f, -1.0f, 1.0f, "%.2f");
			ImGui::DragFloat3("Directional Colour", dirColour, 0.01f, 0.0f, 1.0f, "%.2f");
		}
		dirLight->setDirection(dirDirection[0], dirDirection[1], dirDirection[2]);
		dirLight->setDiffuseColour(dirColour[0], dirColour[1], dirColour[2], 0.7f);
		dirPosition = dirLight->getPosition();

		// Point light 1
		if (ImGui::CollapsingHeader("Point Light 1"))
		{
			ImGui::DragFloat3("Point Position", p1Pos, 0.5f, 0.0f, 100.0f, "%.1f");
			ImGui::DragFloat3("Point Colour", p1Rgb, 0.01f, 0.0f, 1.0f, "%.2f");
			ImGui::DragFloat("Point Specular", &p1Specular, 0.1f, 0.0f, 1.0f, "%.1f");
		}
		pLight1->setPosition(p1Pos[0], p1Pos[1], p1Pos[2]);
		pLight1->setDiffuseColour(p1Rgb[0], p1Rgb[1], p1Rgb[2], 0.7f);

		// Point light 2
		if (ImGui::CollapsingHeader("Point Light 2"))
		{
			ImGui::DragFloat3("Point Position ", p2Pos, 0.5f, 0.0f, 100.0f, "%.1f");
			ImGui::DragFloat3("Point Colour ", p2Rgb, 0.01f, 0.0f, 1.0f, "%.2f");
			ImGui::DragFloat("Point Specular ", &p2Specular, 0.1f, 0.0f, 1.0f, "%.1f");
		}
		pLight2->setPosition(p2Pos[0], p2Pos[1], p2Pos[2]);
		pLight2->setDiffuseColour(p2Rgb[0], p2Rgb[1], p2Rgb[2], 0.7f);

		// Spotlight
		if (ImGui::CollapsingHeader("Spotlight"))
		{
			ImGui::DragFloat3("Spotlight Position", spotPosition, 0.5f, 0.0f, 100.0f, "%.1f");
			ImGui::DragFloat3("Direction ", spotDirection, 0.01f, -1.0f, 1.0f, "%.2f");
			ImGui::DragFloat3("Spotlight Colour", spotColour, 0.01f, 0.0f, 1.0f, "%.2f");
			ImGui::DragFloat("Cutoff", &cutoff, 0.1f, 0.0f, 90.0f, "%.1f");
		}
		spotLight->setPosition(spotPosition[0], spotPosition[1], spotPosition[2]);
		spotLight->setDirection(spotDirection[0], spotDirection[1], spotDirection[2]);
		spotLight->setDiffuseColour(spotColour[0], spotColour[1], spotColour[2], 0.7f);
	}

	// Tesselation
	if (ImGui::CollapsingHeader("Tesselation")) 
	{
		ImGui::DragFloat("Heightmap Tessellation", &heightMapTess, 1.0f, 1.0f, 64.0f, "%.1f");
		ImGui::DragFloat("Wave Tessellation", &waveTess, 1.0f, 1.0f, 64.0f, "%.1f");
	}

	// Post process
	if (ImGui::CollapsingHeader("Depth of Field")) 
	{
		ImGui::DragFloat("Tolerance", &DoFTolerance, 0.1f, 0.1f, 3.0f);
		ImGui::Checkbox("Toggle", &DoFSwitch);
	}

	// Render UI
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}