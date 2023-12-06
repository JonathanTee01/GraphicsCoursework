// Light pixel shader
// Calculate diffuse lighting for a single directional light (also texturing)
#include "LightHeader.hlsli"

Texture2D texture0 : register(t0);
Texture2D depth0 : register(t1);
Texture2D depth1 : register(t2);
SamplerState sampler0 : register(s0);
SamplerState shadowSampler0 : register(s1);

cbuffer DirLightBuffer : register(b0)
{
    float4 dirDiffuseColour;
    float3 dirLightDirection;
    float padding;
};

cbuffer P1LightBuffer : register(b1)
{
    float4 p1DiffuseColour;
    float3 p1Position;
    float p1Specular;
}

cbuffer P2LightBuffer : register(b2)
{
    float4 p2DiffuseColour;
    float3 p2Position;
    float p2Specular;
}

cbuffer SpotLightBuffer : register(b3)
{
    float4 spotDiffuseColour;
    float3 spotDirection;
    float spotCutoff;
    float3 spotPosition;
    float padding1;
}

cbuffer CameraBuffer : register(b4)
{
    float3 cameraPosition;
    float padding2;
}

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 worldPosition : TEXCOORD1;
    
    float4 lightViewPos1 : TEXCOORD2;
    float4 lightViewPos2 : TEXCOORD3;
};

float4 main(InputType input) : SV_TARGET
{
    float4 textureColour;
    float4 lightColour;
    
    // Shadows    
    float shadowBias = 0.007f;
    float4 shadow1Colour = shadowCalculation(input.lightViewPos1, depth0, shadowBias, dirLightDirection, dirDiffuseColour, input.normal, shadowSampler0);
    float4 shadow2Colour = projectionShadowCalculation(input.lightViewPos2, depth1, shadowBias, dirLightDirection, dirDiffuseColour, input.normal, shadowSampler0);
    
	// Sample the texture. Calculate light intensity and colour, return light*texture for final pixel colour.
    // This time we give specular values
    textureColour = texture0.Sample(sampler0, input.tex);
    lightColour = calculateDirectionalLighting(-dirLightDirection, input.normal, dirDiffuseColour) * shadow1Colour;
    lightColour += calculatePointLighting(p1DiffuseColour, p1Position, input.worldPosition, cameraPosition, input.normal, 9.0f, 5.0f, p1Specular);
    lightColour += calculatePointLighting(p2DiffuseColour, p2Position, input.worldPosition, cameraPosition, input.normal, 9.0f, 5.0f, p2Specular);
    lightColour += calculateSpotLighting(spotDiffuseColour, spotPosition, -spotDirection, input.worldPosition, input.normal, spotCutoff) * shadow2Colour;
    
    return textureColour * lightColour;

}



