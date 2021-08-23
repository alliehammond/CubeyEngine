#include "EnginePCH.h"
#include "Graphics\Materials\UIElementMaterial.h"

ID3D11Buffer *UIElementMaterial::constantBufferUIElementMaterial = 0;
ConstantBufferUIElementMaterial UIElementMaterial::cBufStruct = ConstantBufferUIElementMaterial();

UIElementMaterial::UIElementMaterial() : Material(), texture(0), posX(0.0f), posY(0.0f), width(1.0f), height(1.0f)
{
    constantBuffer = constantBufferUIElementMaterial;
}

UIElementMaterial::UIElementMaterial(std::string VS, std::string PS, InputLayout IL, std::string name_, std::string texName, float posX_, float posY_, float width_, float height_)
    : Material(VS, PS, IL, name_), texture(0), posX(posX_), posY(posY_), width(width_), height(height_)
{
    //Initialize constant buffer if it hasn't been already
    if(!constantBufferUIElementMaterial)
    {
        D3D11_BUFFER_DESC constantBufferDesc;
        ZeroMemory(&constantBufferDesc, sizeof(D3D11_BUFFER_DESC));

        constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        constantBufferDesc.ByteWidth = sizeof(ConstantBufferUIElementMaterial);
        constantBufferDesc.CPUAccessFlags = 0;
        constantBufferDesc.Usage = D3D11_USAGE_DEFAULT;

        auto hr = GraphicsSystem::GetD3DDevice()->CreateBuffer(&constantBufferDesc, nullptr, &constantBufferUIElementMaterial);
        if(FAILED(hr))
            LOGERROR("Failed to create constant buffer!");
    }
    constantBuffer = constantBufferUIElementMaterial;

    texture = GraphicsSystem::GetTexture(texName);
}

void UIElementMaterial::BindConstantBuffer(DirectX::XMMATRIX *worldMatrix, DirectX::XMMATRIX *rotationMatrix)
{
    cBufStruct.posSize = XMFLOAT4(posX, posY, width, height);

    GraphicsSystem::GetD3DDeviceContext()->UpdateSubresource(constantBufferUIElementMaterial, 0, nullptr, &cBufStruct, 0, 0);
}

void UIElementMaterial::BindMaterial()
{
    //Texture should always be initialized (to error texture if texture couldn't be found)
    texture->BindAsTexture();
    Material::BindMaterial();
}

Material *UIElementMaterial::Clone()
{
    UIElementMaterial *mat = new UIElementMaterial;
    (*mat) = (*this);
    return mat;
}
