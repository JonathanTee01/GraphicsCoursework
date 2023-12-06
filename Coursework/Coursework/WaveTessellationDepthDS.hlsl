// Tessellation domain shader
// After tessellation the domain shader processes the all the vertices
cbuffer MatrixBuffer : register(b0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

cbuffer TessBuffer : register(b1)
{
    float tessFactor;
    float resolution;
    float2 padding;
}

cbuffer waveBuffer : register(b2)
{
    float time;
    float amplitude;
    float frequency;
    float speed;
}

struct ConstantOutputType
{
    float edges[4] : SV_TessFactor;
    float inside[2] : SV_InsideTessFactor;
};

struct InputType
{
    float3 position : POSITION;
    float2 tex : TEXCOORD0;
};

struct OutputType
{
    float4 position : SV_POSITION;
    float4 depthPosition : TEXCOORD0;
};

[domain("quad")]
OutputType main(ConstantOutputType input, float2 uvwCoord : SV_DomainLocation, const OutputPatch<InputType, 4> patch)
{
    float3 vertexPosition;
    OutputType output;
 
    // Determine the position of the new vertex.
    float3 vertex1 = lerp(patch[0].position, patch[1].position, uvwCoord.y);
    float3 vertex2 = lerp(patch[3].position, patch[2].position, uvwCoord.y);
    vertexPosition = lerp(vertex1, vertex2, uvwCoord.x);
    
    // Calculate the height 
    vertexPosition.y += amplitude * sin(frequency * vertexPosition.x + time) * 0.3;
    vertexPosition.y += amplitude * sin(frequency * vertexPosition.z - time);
    
    // Calculate the position of the new vertex against the world, view, and projection matrices.
    output.position = mul(float4(vertexPosition, 1.0f), worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
    output.depthPosition = output.position;

    return output;
}

