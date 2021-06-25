Texture2D shaderTexture : register(t0);
SamplerState sampleState : register(s0);

struct PixelShaderInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

float4 main(PixelShaderInput IN) : SV_TARGET
{
    return shaderTexture.Sample(sampleState, IN.uv);
}
