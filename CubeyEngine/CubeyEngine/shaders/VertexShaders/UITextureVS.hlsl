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
    float2 uv : TEXCOORD;
};

struct VertexShaderOutput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

//Render UI plane over whole screen
VertexShaderOutput main(VertexData IN)
{
    VertexShaderOutput OUT;

    OUT.position = float4(IN.position.x, IN.position.y, 1.0f, 1.0f);
    OUT.uv = IN.uv;

    return OUT;
}
