// Tessellation domain shader
// After tessellation the domain shader processes the all the vertices
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
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 worldPosition : TEXCOORD1;
    
    float4 lightViewPos1 : TEXCOORD2;
    float4 lightViewPos2 : TEXCOORD3;
};

[domain("quad")]
OutputType main(ConstantOutputType input, float2 uvwCoord : SV_DomainLocation, const OutputPatch<InputType, 4> patch)
{
    float3 vertexPosition;
    float2 texPosition;
    OutputType output;
 
    // Determine the position of the new vertex.
    float3 vertex1 = lerp(patch[0].position, patch[1].position, uvwCoord.y);
    float3 vertex2 = lerp(patch[3].position, patch[2].position, uvwCoord.y);
    vertexPosition = lerp(vertex1, vertex2, uvwCoord.x);
    
    // Determine the tex of the new vertex
    float2 tex1 = lerp(patch[0].tex, patch[1].tex, uvwCoord.y);
    float2 tex2 = lerp(patch[3].tex, patch[2].tex, uvwCoord.y);
    texPosition = lerp(tex1, tex2, uvwCoord.x);
    
    // Calculate the height 
    vertexPosition.y += amplitude * sin(frequency * vertexPosition.x + time) * 0.3;
    vertexPosition.y += amplitude * sin(frequency * vertexPosition.z - time);
	
    // Calculate the new normal
    float3 newNorm = { 0.0f, 1.0f, 0.0f };
    newNorm.x -= frequency * amplitude * cos(frequency * vertexPosition.x + time) * 0.3;
    newNorm.z -= frequency * amplitude * cos(frequency * vertexPosition.z - time);
    
    // Calculate the position of the new vertex against the world, view, and projection matrices.
    output.position = mul(float4(vertexPosition, 1.0f), worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    output.worldPosition = mul(float4(vertexPosition, 1.0f), worldMatrix).xyz;
    
    // Set tex coord and normals
    output.tex = texPosition;
    output.normal = mul(newNorm, (float3x3) worldMatrix);
    output.normal = normalize(output.normal);

    // Set light values
    output.lightViewPos1 = mul(float4(vertexPosition, 1.0f), worldMatrix);
    output.lightViewPos1 = mul(output.lightViewPos1, lightViewMatrix1);
    output.lightViewPos1 = mul(output.lightViewPos1, lightProjectionMatrix1);
    
    output.lightViewPos2 = mul(float4(vertexPosition, 1.0f), worldMatrix);
    output.lightViewPos2 = mul(output.lightViewPos2, lightViewMatrix2);
    output.lightViewPos2 = mul(output.lightViewPos2, lightProjectionMatrix2);

    return output;
}

