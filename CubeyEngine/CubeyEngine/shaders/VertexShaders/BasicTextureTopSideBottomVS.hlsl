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
    float direction : DIRECTION; //0 - top, 1 - side, 2 - bottom
    float3 norm : NORMAL;
};

VertexShaderOutput main(VertexData IN)
{
    VertexShaderOutput OUT;

    matrix mvp = mul(projectionMatrix, mul(viewMatrix, worldMatrix));
    OUT.position = mul(mvp, float4(IN.position, 1.0f));
    OUT.uv = IN.uv;

    OUT.norm = mul(rotationMatrix, float4(IN.normal, 0.0f)).xyz;
    OUT.worldPosition = mul(worldMatrix, float4(IN.position, 1.0f)).xyz;

    //Assume block normals are always in the format (1, 0, 0) || (0, 1, 0) || (0, 0, 1)
    //Calculate direction from normals while avoiding if statements
    int dir = int(IN.normal.y) + int(abs(IN.normal.x)) * 2 + int(abs(IN.normal.z)) * 2; //1 - top, 2 - side, -1 - bottom
    OUT.direction = abs(dir - 1);

    return OUT;
}
