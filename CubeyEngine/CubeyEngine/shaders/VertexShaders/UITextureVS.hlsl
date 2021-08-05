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

    OUT.position = float4(IN.position.x, IN.position.y, 0.0f, 1.0f);
    OUT.uv = IN.uv;

    return OUT;
}
