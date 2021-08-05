#include "EnginePCH.h"
#include "Graphics\Materials\TextureMaterial.h"

ID3D11Buffer *TextureMaterial::constantBufferTextureMaterial = 0;
ConstantBufferTextureMaterial TextureMaterial::cBufStruct = ConstantBufferTextureMaterial();

TextureMaterial::TextureMaterial() : Material(), texture(0), shininess(0.0f)
{
    constantBuffer = constantBufferTextureMaterial;
}

TextureMaterial::TextureMaterial(std::string VS, std::string PS, InputLayout IL, std::string name_, std::string texName, CBY::Vector specularReflection_, float shininess_) 
    : Material(VS, PS, IL, name_), texture(0), specularReflection(specularReflection_), shininess(shininess_)
{
    //Initialize constant buffer if it hasn't been already
    if(!constantBufferTextureMaterial)
    {
        D3D11_BUFFER_DESC constantBufferDesc;
        ZeroMemory(&constantBufferDesc, sizeof(D3D11_BUFFER_DESC));

        constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        constantBufferDesc.ByteWidth = sizeof(ConstantBufferTextureMaterial);
        constantBufferDesc.CPUAccessFlags = 0;
        constantBufferDesc.Usage = D3D11_USAGE_DEFAULT;

        auto hr = GraphicsSystem::GetD3DDevice()->CreateBuffer(&constantBufferDesc, nullptr, &constantBufferTextureMaterial);
        if(FAILED(hr))
            LOGERROR("Failed to create constant buffer!");
    }
    constantBuffer = constantBufferTextureMaterial;

    texture = GraphicsSystem::GetTexture(texName);
}

void TextureMaterial::BindConstantBuffer(DirectX::XMMATRIX *worldMatrix, DirectX::XMMATRIX *rotationMatrix)
{
    cBufStruct.worldMatrix = *worldMatrix;
    cBufStruct.rotationMatrix = *rotationMatrix;
    cBufStruct.ks = XMFLOAT4(specularReflection.x, specularReflection.y, specularReflection.z, 0.0f);
    cBufStruct.shininess = XMFLOAT4(shininess, 0.0f, 0.0f, 0.0f);

    GraphicsSystem::GetD3DDeviceContext()->UpdateSubresource(constantBufferTextureMaterial, 0, nullptr, &cBufStruct, 0, 0);
}

void TextureMaterial::BindMaterial()
{
    //Texture should always be initialized (to error texture if texture couldn't be found)
    texture->BindAsTexture();
    Material::BindMaterial();
}

Material* TextureMaterial::Clone()
{
    TextureMaterial* mat = new TextureMaterial;
    (*mat) = (*this);
    return mat;
}
