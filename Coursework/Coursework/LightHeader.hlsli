// Calculate lighting intensity based on direction and normal. Combine with light colour.
float4 calculateDirectionalLighting(float3 lightDirection, float3 normal, float4 diffuse)
{
    float intensity = saturate(dot(normal, lightDirection));
    float4 colour = saturate(diffuse * intensity);
    return colour;
}

// Calculate lighting intensity for a point light. Specular is added but is pressumed off unless sepcified.
float4 calculatePointLighting(float4 lightColour, float3 lightPos, float3 worldPos, float3 cameraPos, float3 normal, float range, float specExp = 0, float specInt = 0)
{    
    float3 lightVector = normalize(lightPos - worldPos);
    float3 cameraVector = normalize(cameraPos - worldPos);
    
    // Diffuse lighting
    float4 colour = calculateDirectionalLighting(lightVector, normal, lightColour);
    
    // Attenuation
    float distToLight = length(lightPos - worldPos);
    float distToNorm = 1.0 - saturate(distToLight * (1 - (range / 10)));
    colour *= distToNorm;
    
    // Specular
    if (specInt > 0 && specExp > 0)
    {
        float3 mid = normalize(cameraVector + lightVector);
        float normViewAngle = saturate(dot(mid, normal));
        if (normViewAngle > 0.99f)
        {
            colour += lightColour * pow(normViewAngle, specExp) * specInt * distToNorm;
        }
    }
    
    return colour;
}

float4 calculateSpotLighting(float4 lightColour, float3 lightPos, float3 lightDirection, float3 worldPos, float3 normal, float cutoff)
{
    // Becuase it isn't happy with a 0 value return no colour
    if (lightDirection.x == 0 && lightDirection.y == 0 && lightDirection.z == 0)
    {
        return float4(0.0f, 0.0f, 0.0f, 0.0f);
    }
    
    // Calculate the vector between the pixel and the camera
    float3 lightVector = normalize(lightPos - worldPos);
    
    // adjust the cutoff from degrees
    cutoff = cutoff / (180 / 3.14);
    
    // Calculate the intensity based on proximity to the light direction
    float intensity = acos(dot(normalize(lightVector), normalize(lightDirection)));
    intensity = (cutoff - intensity) / cutoff;
    
    // Calculate the lighting colour
    // If intensity is below zero we don't want to return negative values
    float4 colour;
    if (intensity < 0)
    {
        colour = float4(0.0f, 0.0f, 0.0f, 0.0f);
    }
    else
    {
        colour = calculateDirectionalLighting(lightVector, normal, lightColour) * intensity;
    }
    
    // Attenuation
    float distToLight = length(lightPos - worldPos);
    float distToNorm = 1.0 - saturate(distToLight * (1 - (9.8 / 10)));
    colour *= distToNorm;
    
    return colour;
}

float4 shadowCalculation(float4 viewPos, Texture2D shadowMap, float bias, float3 lightDir, float4 lightDiff, float3 lightNorm, SamplerState shadowSampler)
{
    float4 tColour = float4(0.2f, 0.2f, 0.3f, 1.f);
    
    // Caclulate the projected texture coordinates
    float2 projTex = viewPos.xy / viewPos.w;
    projTex *= float2(0.5, -0.5);
    projTex += float2(0.5f, 0.5f);
    
    // Exits if the geometry is not in our shadow map
    if (projTex.x < 0.f || projTex.x > 1.f || projTex.y < 0.f || projTex.y > 1.f)
    {
        return tColour;
    }

    // Sample Shadow Map (get depth of geometry)
    float depthValue = shadowMap.Sample(shadowSampler, projTex).r;
    
    // Calculate the depth from the light
    float lightDepthValue = viewPos.z / viewPos.w;
    lightDepthValue -= bias;
    
    // Compare the depth of the shadow map value and the depth of the light to determine whether to shadow or to light this pixel
    if (lightDepthValue < depthValue)
    {
        tColour = calculateDirectionalLighting(-lightDir, lightNorm, lightDiff);
    }
    
    return tColour;
}

float LinearizeDepth(float depth, float near, float far)
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));
}

float4 projectionShadowCalculation(float4 viewPos, Texture2D shadowMap, float bias, float3 lightDir, float4 lightDiff, float3 lightNorm, SamplerState shadowSampler)
{
    float4 tColour = float4(0.2f, 0.2f, 0.3f, 1.f);
    
    // Caclulate the projected texture coordinates
    float2 projTex = viewPos.xy / viewPos.w;
    projTex *= float2(0.5, -0.5);
    projTex += float2(0.5f, 0.5f);
    
    // Exits if the geometry is not in our shadow map
    if (projTex.x < 0.f || projTex.x > 1.f || projTex.y < 0.f || projTex.y > 1.f)
    {
        return tColour;
    }

    // Sample Shadow Map (get depth of geometry)
    float depthValue = shadowMap.Sample(shadowSampler, projTex).r;
    depthValue = LinearizeDepth(depthValue, 0.1f, 150.f);
    
    // Calculate the depth from the light
    float lightDepthValue = viewPos.z / viewPos.w;
    lightDepthValue = LinearizeDepth(lightDepthValue, 0.1f, 150.f);
    lightDepthValue -= bias;
    
    // Compare the depth of the shadow map value and the depth of the light to determine whether to shadow or to light this pixel
    if (lightDepthValue < depthValue)
    {
        tColour = calculateDirectionalLighting(-lightDir, lightNorm, lightDiff);
    }
    
    return tColour;
}