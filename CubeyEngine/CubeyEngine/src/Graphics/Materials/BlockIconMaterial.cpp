#include "EnginePCH.h"
#include "Graphics\Materials\BlockIconMaterial.h"

ID3D11Buffer *BlockIconMaterial::constantBufferBlockIconMaterial = 0;
ConstantBufferBlockIconMaterial BlockIconMaterial::cBufStruct = ConstantBufferBlockIconMaterial();

BlockIconMaterial::BlockIconMaterial() : Material(), texture(0), bType(BlockType::Air), posX(0.0f), posY(0.0f), width(1.0f), height(1.0f)
{
    constantBuffer = constantBufferBlockIconMaterial;
}

BlockIconMaterial::BlockIconMaterial(std::string VS, std::string PS, InputLayout IL, std::string name_, std::string texName, BlockType bType_, float posX_, float posY_, float width_, float height_)
    : Material(VS, PS, IL, name_), texture(0), bType(bType_), posX(posX_), posY(posY_), width(width_), height(height_)
{
    //Initialize constant buffer if it hasn't been already
    if(!constantBufferBlockIconMaterial)
    {
        D3D11_BUFFER_DESC constantBufferDesc;
        ZeroMemory(&constantBufferDesc, sizeof(D3D11_BUFFER_DESC));

        constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        constantBufferDesc.ByteWidth = sizeof(ConstantBufferBlockIconMaterial);
        constantBufferDesc.CPUAccessFlags = 0;
        constantBufferDesc.Usage = D3D11_USAGE_DEFAULT;

        auto hr = GraphicsSystem::GetD3DDevice()->CreateBuffer(&constantBufferDesc, nullptr, &constantBufferBlockIconMaterial);
        if(FAILED(hr))
            LOGERROR("Failed to create constant buffer!");
    }
    constantBuffer = constantBufferBlockIconMaterial;

    texture = GraphicsSystem::GetTexture(texName);
}

void BlockIconMaterial::BindConstantBuffer(DirectX::XMMATRIX *worldMatrix, DirectX::XMMATRIX *rotationMatrix)
{
    cBufStruct.posSize = XMFLOAT4(posX, posY, width, height);
    cBufStruct.blockTypeNumBlocks = XMFLOAT4(static_cast<float>(bType), static_cast<float>(BlockType::BLOCKCOUNT), static_cast<float>(blockIconSelected), 0.0f);

    GraphicsSystem::GetD3DDeviceContext()->UpdateSubresource(constantBufferBlockIconMaterial, 0, nullptr, &cBufStruct, 0, 0);
}

void BlockIconMaterial::BindMaterial()
{
    //Texture should always be initialized (to error texture if texture couldn't be found)
    texture->BindAsTexture();
    Material::BindMaterial();
}

Material *BlockIconMaterial::Clone()
{
    BlockIconMaterial *mat = new BlockIconMaterial;
    (*mat) = (*this);
    return mat;
}
