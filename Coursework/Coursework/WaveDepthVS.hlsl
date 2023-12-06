// Light vertex shader
// Standard issue vertex shader, apply matrices, pass info to pixel shader
Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

cbuffer MatrixBuffer : register(b0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
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
    float4 depthPosition : TEXCOORD0;
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
    
	// Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
    output.depthPosition = output.position;

    return output;
}