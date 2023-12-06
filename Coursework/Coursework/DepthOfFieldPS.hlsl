// Texture pixel/fragment shader
// Basic fragment shader for rendering textured geometry

// Texture and sampler registers
Texture2D texture0 : register(t0);
Texture2D texture1 : register(t1);
SamplerState Sampler0 : register(s0);

cbuffer screenBuffer : register(b0)
{
    float screenWidth;
    float screenHeight;
    float tolerance;
    float on;
}

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};


float4 main(InputType input) : SV_TARGET
{
    if (!on)
    {
        float4 colour = texture0.Sample(Sampler0, input.tex); 
        return colour;
    }
    
	// Sample the pixel color from the texture using the sampler at this texture coordinate location.
    float3 closestDepth;
    
    // Test the depth value for the centre pixel
    float3 centreDepth = texture1.Sample(Sampler0, float2(0.5f, 0.5f));
    closestDepth = centreDepth;
    
    // Test the depth value for slightly to the left of the centre pixel
    // Use this value as the closest point if it is closer
    float3 leftDepth = texture1.Sample(Sampler0, float2(0.48f, 0.5f));
    if (leftDepth.r > closestDepth.r)
    {
        closestDepth = leftDepth;
    }
    
    // Test the depth value for slightly to the right of the centre pixel
    // Use this value as the closest point if it is closer
    float3 rightDepth = texture1.Sample(Sampler0, float2(0.52f, 0.5f));
    if (rightDepth.r > closestDepth.r)
    {
        closestDepth = rightDepth;
    }
    
    // Test the depth value for slightly up (technically down because texture y is inverted) of the centre pixel
    // Use this value as the closest point if it is closer
    float3 upDepth = texture1.Sample(Sampler0, float2(0.5f, 0.52f));
    if (upDepth.r > closestDepth.r)
    {
        closestDepth = upDepth;
    }
    
    // Test the depth value for slightly down (technically up because texture y is inverted) of the centre pixel
    // Use this value as the closest point if it is closer
    float3 downDepth = texture1.Sample(Sampler0, float2(0.5f, 0.48f));
    if (downDepth.r > closestDepth.r)
    {
        closestDepth = downDepth;
    }
    
    // Find the depth value of the pixel in question
    float3 pixelDepth = texture1.Sample(Sampler0, input.tex);
    
    // Calcuate the difference in depth
    float depthDiff = closestDepth.r - pixelDepth.r;
    depthDiff *= 300;
    
    float4 colour;
    float texelWidthSize = 1.0f / screenWidth;
    float texelHeightSize = 1.0f / screenHeight;
        
    // If statements to apply blure based on how much closer/further it is from the closest viewed pixel
    if (depthDiff < -0.3f * tolerance || depthDiff > 0.3f * tolerance)
    {
        // Horizonatal blur
        colour = texture0.Sample(Sampler0, input.tex + float2(texelWidthSize * 5.0f, 0.0f)) * 0.0064f;
        colour += texture0.Sample(Sampler0, input.tex + float2(texelWidthSize * 4.0f, 0.0f)) * 0.0128f;
        colour += texture0.Sample(Sampler0, input.tex + float2(texelWidthSize * 3.0f, 0.0f)) * 0.0256f;
        colour += texture0.Sample(Sampler0, input.tex + float2(texelWidthSize * 2.0f, 0.0f)) * 0.0513f;
        colour += texture0.Sample(Sampler0, input.tex + float2(texelWidthSize * 1.0f, 0.0f)) * 0.1026f;
        colour += texture0.Sample(Sampler0, input.tex) * 0.2051f;
        colour += texture0.Sample(Sampler0, input.tex + float2(texelWidthSize * -1.0f, 0.0f)) * 0.1026f;
        colour += texture0.Sample(Sampler0, input.tex + float2(texelWidthSize * -2.0f, 0.0f)) * 0.0513f;
        colour += texture0.Sample(Sampler0, input.tex + float2(texelWidthSize * -3.0f, 0.0f)) * 0.0256f;
        colour += texture0.Sample(Sampler0, input.tex + float2(texelWidthSize * -4.0f, 0.0f)) * 0.0128f;
        colour += texture0.Sample(Sampler0, input.tex + float2(texelWidthSize * -5.0f, 0.0f)) * 0.0064f;
        
        // Vertical blur
        colour += texture0.Sample(Sampler0, input.tex + float2(0.0f, texelHeightSize * 5.0f)) * 0.0064f;
        colour += texture0.Sample(Sampler0, input.tex + float2(0.0f, texelHeightSize * 4.0f)) * 0.0128f;
        colour += texture0.Sample(Sampler0, input.tex + float2(0.0f, texelHeightSize * 3.0f)) * 0.0256f;
        colour += texture0.Sample(Sampler0, input.tex + float2(0.0f, texelHeightSize * 2.0f)) * 0.0513f;
        colour += texture0.Sample(Sampler0, input.tex + float2(0.0f, texelHeightSize * 1.0f)) * 0.1026f;
        colour += texture0.Sample(Sampler0, input.tex + float2(0.0f, texelHeightSize * -1.0f)) * 0.1026f;
        colour += texture0.Sample(Sampler0, input.tex + float2(0.0f, texelHeightSize * -2.0f)) * 0.0513f;
        colour += texture0.Sample(Sampler0, input.tex + float2(0.0f, texelHeightSize * -3.0f)) * 0.0256f;
        colour += texture0.Sample(Sampler0, input.tex + float2(0.0f, texelHeightSize * -4.0f)) * 0.0128f;
        colour += texture0.Sample(Sampler0, input.tex + float2(0.0f, texelHeightSize * -5.0f)) * 0.0064f;
    }
    else if (depthDiff < -0.2f * tolerance || depthDiff > 0.2f * tolerance)
    {
        // Horizonatal blur
        colour = texture0.Sample(Sampler0, input.tex + float2(texelWidthSize * 3.0f, 0.0f)) * 0.0278f;
        colour += texture0.Sample(Sampler0, input.tex + float2(texelWidthSize * 2.0f, 0.0f)) * 0.0556f;
        colour += texture0.Sample(Sampler0, input.tex + float2(texelWidthSize * 1.0f, 0.0f)) * 0.1111f;
        colour += texture0.Sample(Sampler0, input.tex) * 0.2222f;
        colour += texture0.Sample(Sampler0, input.tex + float2(texelWidthSize * -1.0f, 0.0f)) * 0.1111f;
        colour += texture0.Sample(Sampler0, input.tex + float2(texelWidthSize * -2.0f, 0.0f)) * 0.0556f;
        colour += texture0.Sample(Sampler0, input.tex + float2(texelWidthSize * -3.0f, 0.0f)) * 0.0278f;
        
        // Vertical blur
        colour += texture0.Sample(Sampler0, input.tex + float2(0.0f, texelHeightSize * 3.0f)) * 0.0278f;
        colour += texture0.Sample(Sampler0, input.tex + float2(0.0f, texelHeightSize * 2.0f)) * 0.0556f;
        colour += texture0.Sample(Sampler0, input.tex + float2(0.0f, texelHeightSize * 1.0f)) * 0.1111f;
        colour += texture0.Sample(Sampler0, input.tex + float2(0.0f, texelHeightSize * -1.0f)) * 0.1111f;
        colour += texture0.Sample(Sampler0, input.tex + float2(0.0f, texelHeightSize * -2.0f)) * 0.0556f;
        colour += texture0.Sample(Sampler0, input.tex + float2(0.0f, texelHeightSize * -3.0f)) * 0.0278f;
    }
    else if (depthDiff < -0.1f * tolerance || depthDiff > 0.1f * tolerance)
    {
        // Horizonatal blur
        colour = texture0.Sample(Sampler0, input.tex + float2(texelWidthSize * 1.0f, 0.0f)) * 0.125f;
        colour += texture0.Sample(Sampler0, input.tex) * 0.5f;
        colour += texture0.Sample(Sampler0, input.tex + float2(texelWidthSize * -1.0f, 0.0f)) * 0.125f;
        
        // Vertical blur
        colour += texture0.Sample(Sampler0, input.tex + float2(0.0f, texelHeightSize * 1.0f)) * 0.125;
        colour += texture0.Sample(Sampler0, input.tex + float2(0.0f, texelHeightSize * -1.0f)) * 0.125;
    }
    else
    {
        colour = texture0.Sample(Sampler0, input.tex);
    }
    
    return colour;
}