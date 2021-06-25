cbuffer Application : register(b0)
{
    matrix projectionMatrix;
}
cbuffer Frame : register(b1)
{
    matrix viewMatrix;
}
cbuffer Object : register(b2)
{
    matrix worldMatrix;
}

struct VertexData
{
    float3 position : POSITION;
    float3 color: COLOR;
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
    OUT.color = float4(IN.color, 1.0f);

    return OUT;
}