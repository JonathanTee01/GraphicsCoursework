// Tessellation Hull Shader
// Prepares control points for tessellation

cbuffer TessBuffer : register(b0)
{
    float tessFactor;
    float3 padding;
}

struct InputType
{
    float3 position : POSITION;
    float2 tex : TEXCOORD0;
};

struct ConstantOutputType
{
    float edges[4] : SV_TessFactor;
    float inside[2] : SV_InsideTessFactor;
};

struct OutputType
{
    float3 position : POSITION;
    float2 tex : TEXCOORD0;
};

ConstantOutputType PatchConstantFunction(InputPatch<InputType, 4> inputPatch, uint patchId : SV_PrimitiveID)
{
    ConstantOutputType output;
    
    // Set the tessellation factors for the four edges of the quad.
    output.edges[0] = tessFactor;
    output.edges[1] = tessFactor;
    output.edges[2] = tessFactor;
    output.edges[3] = tessFactor;

    // Set the tessellation factor for tessallating inside the quad.
    output.inside[0] = tessFactor;
    output.inside[1] = tessFactor;

    return output;
}


[domain("quad")]
[partitioning("integer")]
[outputtopology("triangle_ccw")]
[outputcontrolpoints(4)]
[patchconstantfunc("PatchConstantFunction")]
[maxtessfactor(64.0f)]
OutputType main(InputPatch<InputType, 4> patch, uint pointId : SV_OutputControlPointID, uint patchId : SV_PrimitiveID)
{
    OutputType output;

    // Set the position for this control point as the output position.
    output.position = patch[pointId].position;
    
    // Set the output tex as the input
    output.tex = patch[pointId].tex;

    return output;
}