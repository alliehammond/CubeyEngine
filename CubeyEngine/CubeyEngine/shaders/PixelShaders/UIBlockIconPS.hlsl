Texture2D shaderTexture : register(t0);
SamplerState sampleState : register(s0);

struct PixelShaderInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

//Uses block icon constant buffer
cbuffer Object : register(b2)
{
    //X position, Y position, width, height of block icon ui element
    //Width, height as percentage of screen size
    //Position between -1 and 1
    float4 posSize;
    //First float - block type, second float - num blocks
    float4 blockTypeNumBlocks;
}

float4 main(PixelShaderInput IN) : SV_TARGET
{
    float blockType = blockTypeNumBlocks.x;
    float numBlocks = blockTypeNumBlocks.y;
    IN.uv.x = IN.uv.x / numBlocks + blockType / numBlocks;
    float4 sampledCol = shaderTexture.Sample(sampleState, IN.uv);

    if(sampledCol.w < 0.1f)discard;
    return sampledCol;
}
