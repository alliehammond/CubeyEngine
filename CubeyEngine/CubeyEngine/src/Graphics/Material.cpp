#include "EnginePCH.h"
#include "Graphics\Material.h"

ID3D11Buffer *Material::constantBufferMaterial = 0;

Material::Material() : vertexSize(0), ILayout(InputLayout::INPUTLAYOUTCOUNT), constantBuffer(constantBufferMaterial)
{ }

Material::Material(std::string VS, std::string PS, InputLayout IL, std::string name_)
{
    //Initialize constant buffer if it hasn't been already
    if(!constantBufferMaterial)
    {
        D3D11_BUFFER_DESC constantBufferDesc;
        ZeroMemory(&constantBufferDesc, sizeof(D3D11_BUFFER_DESC));

        constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        constantBufferDesc.ByteWidth = sizeof(DirectX::XMMATRIX);
        constantBufferDesc.CPUAccessFlags = 0;
        constantBufferDesc.Usage = D3D11_USAGE_DEFAULT;

        auto hr = GraphicsSystem::GetD3DDevice()->CreateBuffer(&constantBufferDesc, nullptr, &constantBufferMaterial);
        if(FAILED(hr))
            LOGERROR("Failed to create constant buffer!");
    }
    constantBuffer = constantBufferMaterial;

    pVertShader = GraphicsSystem::GetVertexShader(VS);
    pPixShader = GraphicsSystem::GetPixelShader(PS);
    pInputLayout = GraphicsSystem::GetInputLayout(IL);
    name = name_;
    vertexSize = InputLayoutVertexSizes[IL];
    ILayout =  IL;
}

void Material::BindConstantBuffer(DirectX::XMMATRIX *worldMatrix, DirectX::XMMATRIX *rotationMatrix)
{
    GraphicsSystem::GetD3DDeviceContext()->UpdateSubresource(constantBufferMaterial, 0, nullptr, worldMatrix, 0, 0);
}

void Material::BindMaterial()
{
    GraphicsSystem::GetD3DDeviceContext()->IASetInputLayout(pInputLayout);
    GraphicsSystem::GetD3DDeviceContext()->VSSetShader(pVertShader, nullptr, 0);
    GraphicsSystem::GetD3DDeviceContext()->PSSetShader(pPixShader, nullptr, 0);
}

Material* Material::Clone()
{
    Material *mat = new Material;
    (*mat) = (*this);
    return mat;
}
