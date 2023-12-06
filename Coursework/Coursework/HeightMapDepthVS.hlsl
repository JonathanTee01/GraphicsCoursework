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
};

struct OutputType
{
    float4 position : SV_POSITION;
    float4 depthPosition : TEXCOORD0;
};

float GetHeight(float2 uv)
{
    // Sample the height map colour and use the red channel to calcualte height
    // Any channel could be used as it is greyscale
    float4 colour = texture0.SampleLevel(sampler0, uv, 0);
    float height = colour.x;
    
    // Height is currently between 0 and 1 so multiply by 30 to give the plane noticable depth
    return height * 30;
}

OutputType main(InputType input)
{
    OutputType output;

    // Scale the y position by the texture colour using a function
    input.position.y += GetHeight(input.tex.xy);
    
	// Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
    output.depthPosition = output.position;

    return output;
}