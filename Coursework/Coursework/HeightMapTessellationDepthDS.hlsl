// Tessellation domain shader
// After tessellation the domain shader processes the all the vertices
Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

cbuffer MatrixBuffer : register(b0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

struct ConstantOutputType
{
    float edges[4] : SV_TessFactor;
    float inside[2] : SV_InsideTessFactor;
};

struct InputType
{
    float3 position : POSITION;
    float2 tex : TEXCOORD1;
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

[domain("quad")]
OutputType main(ConstantOutputType input, float2 uvCoord : SV_DomainLocation, const OutputPatch<InputType, 4> patch)
{
    float3 vertexPosition;
    float2 texPosition;
    OutputType output;
 
    // Determine the position of the new vertex.
    float3 vertex1 = lerp(patch[0].position, patch[1].position, uvCoord.y);
    float3 vertex2 = lerp(patch[3].position, patch[2].position, uvCoord.y);
    vertexPosition = lerp(vertex1, vertex2, uvCoord.x);
    
    // Determine the tex of the new vertex
    float2 tex1 = lerp(patch[0].tex, patch[1].tex, uvCoord.y);
    float2 tex2 = lerp(patch[3].tex, patch[2].tex, uvCoord.y);
    texPosition = lerp(tex1, tex2, uvCoord.x);
    
    // Calculate the height
    vertexPosition.y = GetHeight(texPosition);
    
    // Calculate the position of the new vertex against the world, view, and projection matrices.
    output.position = mul(float4(vertexPosition, 1.0f), worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
    output.depthPosition = output.position;

    return output;
}

