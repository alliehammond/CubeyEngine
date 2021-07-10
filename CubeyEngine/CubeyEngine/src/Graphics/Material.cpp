#include "EnginePCH.h"
#include "Graphics\Material.h"

Material::Material() : vertexSize(0)
{ }

Material::Material(std::string VS, std::string PS, InputLayout IL, std::string name_)
{
    pVertShader = GraphicsSystem::GetVertexShader(VS);
    pPixShader = GraphicsSystem::GetPixelShader(PS);
    pInputLayout = GraphicsSystem::GetInputLayout(IL);
    name = name_;
    vertexSize = InputLayoutVertexSizes[IL];
}

void Material::BindMaterial()
{
    GraphicsSystem::GetD3DDeviceContext()->IASetInputLayout(pInputLayout);
    GraphicsSystem::GetD3DDeviceContext()->VSSetShader(pVertShader, nullptr, 0);
    GraphicsSystem::GetD3DDeviceContext()->PSSetShader(pPixShader, nullptr, 0);
}

std::unique_ptr<Material> Material::Clone()
{
    std::unique_ptr<Material> mat = std::make_unique<Material>();
    (*mat) = (*this);
    return mat;
}
