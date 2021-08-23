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
//Uses ui element material
cbuffer Object : register(b2)
{
    //X position, Y position, width, height of block icon ui element
    //Width, height as percentage of screen size
    //Position between -1 and 1
    float4 posSize;
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

//Render UI plane over whole screen (scaled to position of block icon element)
VertexShaderOutput main(VertexData IN)
{
    VertexShaderOutput OUT;

    OUT.position = float4(IN.position.x * posSize.z + posSize.x, IN.position.y * posSize.w + posSize.y, 0.0f, 1.0f);
    OUT.uv = IN.uv;

    return OUT;
}
