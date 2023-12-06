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

cbuffer TimeBufferType : register(b1)
{
    float time;
    float amplitude;
    float frequency;
    float speed;
};

// Wave
OutputType main(InputType input)
{
    OutputType output;
    	
	// Calcualte the height of the wave vertices
    input.position.y += amplitude * sin(frequency * input.position.x + time) * 0.3;
    input.position.y += amplitude * sin(frequency * input.position.z - time);
	
    // Calcualte the new normals
    input.normal.x -= frequency * amplitude * cos(frequency * input.position.x + time) * 0.3;
    input.normal.z -= frequency * amplitude * cos(frequency * input.position.z - time);
    
	// Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
    // Set the light
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