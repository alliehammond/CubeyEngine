#include "EnginePCH.h"
#include "Graphics\Material.h"

Material::Material() : vertexSize(0), ILayout(InputLayout::INPUTLAYOUTCOUNT)
{ }

Material::Material(std::string VS, std::string PS, InputLayout IL, std::string name_)
{
    pVertShader = GraphicsSystem::GetVertexShader(VS);
    pPixShader = GraphicsSystem::GetPixelShader(PS);
    pInputLayout = GraphicsSystem::GetInputLayout(IL);
    name = name_;
    vertexSize = InputLayoutVertexSizes[IL];
    ILayout =  IL;
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
