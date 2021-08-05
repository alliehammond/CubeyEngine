cbuffer Application : register(b0)
{
    matrix projectionMatrix;
}
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

struct VertexData
{
    float3 position : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
};

struct VertexShaderOutput
{
    float4 position : SV_POSITION;
    float3 worldPosition : WORLDPOS;
    float2 uv : TEXCOORD;
    float3 norm : NORMAL;
};

VertexShaderOutput main(VertexData IN)
{
    VertexShaderOutput OUT;

    matrix mvp = mul(projectionMatrix, mul(viewMatrix, worldMatrix));
    OUT.position = mul(mvp, float4(IN.position, 1.0f));
    OUT.norm = mul(rotationMatrix, float4(IN.normal, 0.0f)).xyz;
    OUT.uv = IN.uv;
    OUT.worldPosition = mul(worldMatrix, float4(IN.position, 1.0f)).xyz;

    return OUT;
}
