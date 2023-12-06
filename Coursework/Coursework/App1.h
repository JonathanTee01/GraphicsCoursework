// Application.h
#ifndef _APP1_H
#define _APP1_H

// Includes
#include "DXF.h"	// include dxframework
#include "HeightMapShader.h"
#include "WaveShader.h"
#include "Timer.h"
#include "GenericShader.h"
#include "DepthShader.h"
#include "PostProcessShader.h"
#include "TessellationShader.h"
#include "TessellationPlane.h"
#include "TessellationDepthShader.h"


class App1 : public BaseApplication
{
public:

	App1();
	~App1();
	void init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input* in, bool VSYNC, bool FULL_SCREEN);

	bool frame();

protected:
	bool render();
	void shadowPass();
	void shadowPass2();
	void depthPass();
	void finalPass();
	void postPass();
	void gui();

private:	
	// Shaders
	GenericShader* genericShader;
	TessellationShader* tessShader;
	TessellationDepthShader* tessDepthShader;

	// Height map
	HeightMapShader* heightShader;
	TessellationPlane* tessHeightMesh;
	PlaneMesh* heightMesh;
	float heightMapTess = 1.0f;

	// Model
	AModel* hut;

	// Waves
	WaveShader* waveShader;
	TessellationPlane* tessWaveMesh;
	PlaneMesh* waveMesh;
	float waveTess = 1.0f;
	float waveHeight = 0.5f;
	float waveFrequency = 0.3f;
	float time = 0.0f;

	// Lights
   // Directional
	XMFLOAT3 dirPosition;
	Light* dirLight;
	float dirDirection[3] = {0.7f, -0.7f, 0.25f};
	float dirColour[3] = {0.7f, 0.7f, 0.7f};

   // Point light 1
	SphereMesh* lightMesh1;
	Light* pLight1;
	float p1Pos[3] = { 18.5f, 4.5f, 62.5f };
	float p1Rgb[3] = { 0.0f, 1.0f, 1.0f };
	float p1Specular = 0.2f;

   // Point light 2
	SphereMesh* lightMesh2;
	Light* pLight2;
	float p2Pos[3] = { 14.5f, 5.5f, 57.5f };
	float p2Rgb[3] = { 1.0f, 1.0f, 0.0f };
	float p2Specular = 0.2f;

   // Spotlight
	SphereMesh* lightMesh3;
	Light* spotLight;
	float spotDirection[3] = {0.0f, -1.0f, 0.7f};
	float spotPosition[3] = { 29.0f, 20.0f, 37.0f };
	float spotColour[3] = {1.0f, 0.0f, 1.0f};
	float cutoff = 30;

	// Shadows
	DepthShader* depthShader;
	ShadowMap* shadowMap;
	ShadowMap* shadowMap2;

	// Depth of field
	OrthoMesh* ortho;
	PostProcessShader* postProcessShader;
	RenderTexture* screenTexture;
	ShadowMap* depthMap;
	float width, height, DoFTolerance = 1.0f;
	bool DoFToggle = false;
	bool DoFSwitch = false;
};

#endif