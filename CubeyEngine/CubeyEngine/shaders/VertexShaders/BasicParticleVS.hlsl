cbuffer Application : register(b0)
{
    matrix projectionMatrix;
}
cbuffer Frame : register(b1)
{
    matrix viewMatrix;
    float4 Ia; // Only first 3 floats used
    float4 Ld; // Only first 3 floats used
    float4 Ls; // Only first 3 floats used
    float4 l; // Only first 3 floats used
    float4 eyePos; // Only first 3 floats used
}
//Uses material constant buffer
cbuffer Object : register(b2)
{
    matrix worldMatrix;
}

struct VertexData
{
    float3 position : POSITION;
    float4 colorAlpha : COLOR;
};

struct VertexShaderOutput
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

VertexShaderOutput main(VertexData IN)
{
    VertexShaderOutput OUT;

    matrix mvp = mul(projectionMatrix, mul(viewMatrix, worldMatrix));
    OUT.position = mul(mvp, float4(IN.position, 1.0f));
    OUT.color = IN.colorAlpha;

    return OUT;
}