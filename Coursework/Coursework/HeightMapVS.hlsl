// Light vertex shader
// Standard issue vertex shader, apply matrices, pass info to pixel shader
Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

cbuffer MatrixBuffer : register(b0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
    
    // Light 1
    matrix lightViewMatrix1;
    matrix lightProjectionMatrix1;
    
    // Light 2
    matrix lightViewMatrix2;
    matrix lightProjectionMatrix2;
};

struct InputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};

struct OutputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 worldPosition : TEXCOORD1;
    
    float4 lightViewPos1 : TEXCOORD2;
    float4 lightViewPos2 : TEXCOORD3;
};

float GetHeight(float2 uv)
{
    float4 colour = texture0.SampleLevel(sampler0, uv, 0);
    float height = colour.x;
    return height * 30;
}


OutputType main(InputType input)
{
    OutputType output;

	// Calculate the position of the vertex against the world, view, and projection matrices.

    input.position.y += GetHeight(input.tex.xy);
	
    float3 heightMappedNorm;
	
	// Get heights of local vertexes
    float left = GetHeight(float2(input.tex.x - 0.01f, input.tex.y));
    float right = GetHeight(float2(input.tex.x + 0.01f, input.tex.y));
    float up = GetHeight(float2(input.tex.x, input.tex.y - 0.01f));
    float down = GetHeight(float2(input.tex.x, input.tex.y + 0.01f));
	
    // Simpler calulation for normals that passes for low resolutions
    heightMappedNorm = float3((left - right) / 2, 1.0f, (up - down) / 2);
	
    input.normal = heightMappedNorm;
    
    // Calculate the position of the new vertex against the world, view, and projection matrices.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
    // Light positions
    output.lightViewPos1 = mul(input.position, worldMatrix);
    output.lightViewPos1 = mul(output.lightViewPos1, lightViewMatrix1);
    output.lightViewPos1 = mul(output.lightViewPos1, lightProjectionMatrix1);
    
    output.lightViewPos2 = mul(input.position, worldMatrix);
    output.lightViewPos2 = mul(output.lightViewPos2, lightViewMatrix2);
    output.lightViewPos2 = mul(output.lightViewPos2, lightProjectionMatrix2);
    
	// Store the texture coordinates for the pixel shader.
    output.tex = input.tex;

	// Calculate the normal vector against the world matrix only and normalise.
    output.normal = mul(input.normal, (float3x3) worldMatrix);
    output.normal = normalize(output.normal);
    output.worldPosition = mul(input.position, worldMatrix).xyz;

    return output;
}