Texture2D shaderTexture : register(t0);
SamplerState sampleState : register(s0);

struct PixelShaderInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
    float direction : DIRECTION; //0 - top, 1 - side, 2 - bottom
};

float4 main(PixelShaderInput IN) : SV_TARGET
{
    //Starting offset for texture file depending on direction of block
    float uvXOffset = IN.direction / 3.0f;

    float2 uvCoord = float2(uvXOffset + IN.uv.x * (1.0f / 3.0f), IN.uv.y);

    return shaderTexture.Sample(sampleState, uvCoord);
}
