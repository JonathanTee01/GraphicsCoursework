
cbuffer MatrixBuffer : register(b0)
{
    // Matrices
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
    
    // Light 1
    matrix lightViewMatrix1;
    matrix lightProjectionMatrix1;
    
    // Light 1
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


OutputType main(InputType input)
{
    OutputType output;

	// Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
        
    // Calculate lights
    output.lightViewPos1 = mul(input.position, worldMatrix);
    output.lightViewPos1 = mul(output.lightViewPos1, lightViewMatrix1);
    output.lightViewPos1 = mul(output.lightViewPos1, lightProjectionMatrix1);
    
    output.lightViewPos2 = mul(input.position, worldMatrix);
    output.lightViewPos2 = mul(output.lightViewPos2, lightViewMatrix2);
    output.lightViewPos2 = mul(output.lightViewPos2, lightProjectionMatrix2);
    
    // Store tex value
    output.tex = input.tex;
    
    // Calcualte world position and normal
    output.worldPosition = mul(input.position, worldMatrix).xyz;
    output.normal = mul(input.normal, (float3x3) worldMatrix);
    output.normal = normalize(output.normal);

    return output;
}