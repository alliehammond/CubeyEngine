#include "PhongShading.hlsli"

Texture2D shaderTexture : register(t0);
SamplerState sampleState : register(s0);

cbuffer Frame : register(b1)
{
    matrix viewMatrix;
    float4 Ia;      // Only first 3 floats used
    float4 Ld;      // Only first 3 floats used
    float4 Ls;      // Only first 3 floats used
    float4 l;       // Only first 3 floats used
    float4 eyePos;  // Only first 3 floats used
}
//Uses texture material constant buffer
cbuffer Object : register(b2)
{
    matrix worldMatrix;
    matrix rotationMatrix;
    float4 ks;        // Only first 3 floats used
    float4 shininess; // Only first float used
}

struct PixelShaderInput
{
    float4 position : SV_POSITION;
    float3 worldPosition : WORLDPOS;
    float2 uv : TEXCOORD;
    float direction : DIRECTION; //0 - top, 1 - side, 2 - bottom
    float3 norm : NORMAL;
};

float4 main(PixelShaderInput IN) : SV_TARGET
{
    //Starting offset for texture file depending on direction of block
    float uvXOffset = IN.direction / 3.0f;

    float2 uvCoord = float2(uvXOffset + IN.uv.x * (1.0f / 3.0f), IN.uv.y);

    float4 texSample = shaderTexture.Sample(sampleState, uvCoord);
    if(texSample.w < 0.1f)discard;

    float3 diffuse = texSample.xyz;

    float3 v = eyePos.xyz - IN.worldPosition;

    return float4(CalcPhongShading(Ia.xyz * diffuse, Ld.xyz, Ls.xyz, diffuse, ks.xyz, shininess.x, IN.norm, l.xyz, v), 1.0f);
}
