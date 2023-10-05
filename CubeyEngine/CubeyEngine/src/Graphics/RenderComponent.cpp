#include "EnginePCH.h"
#include "Graphics\RenderComponent.h"

RenderComponent::RenderComponent(std::string modelFileName, GameObject *owner) : Component(owner, "RenderComponent")
{
    //Use default material
    Material mat("BasicVertexShader.cso", "BasicPixelShader.cso", InputLayout::POSCOL, "BaseMaterial");

    pModel = new Model(modelFileName, &mat, mat.GetInputLayout());
}

RenderComponent::RenderComponent(std::string modelFileName, Material* mat, GameObject *owner, CBY::Vector4 colorAlpha) : Component(owner, "RenderComponent")
{
    pModel = new Model(modelFileName, mat, mat->GetInputLayout(), colorAlpha);
    if(colorAlpha.w < 1.0f)transparent = true;
}

RenderComponent::RenderComponent(Model *Model, GameObject* owner)
    : Component(owner, "RenderComponent"), pModel(Model)
{ }

RenderComponent::~RenderComponent()
{
    delete pModel;
}
