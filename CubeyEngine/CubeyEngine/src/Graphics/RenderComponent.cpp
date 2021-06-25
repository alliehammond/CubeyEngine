#include "EnginePCH.h"
#include "Graphics\RenderComponent.h"

RenderComponent::RenderComponent(std::string modelFileName, GameObject *owner) : Component(owner, "RenderComponent")
{
    //Use default material
    Material mat("BasicVertexShader.cso", "BasicPixelShader.cso", InputLayout::POSCOL, "BaseMaterial");

    pModel = new Model(modelFileName, &mat);
}

RenderComponent::RenderComponent(std::string modelFileName, Material* mat, GameObject* owner) : Component(owner, "RenderComponent")
{
    pModel = new Model(modelFileName, mat);
}

RenderComponent::RenderComponent(Model *Model, GameObject* owner)
    : Component(owner, "RenderComponent"), pModel(Model)
{ }

RenderComponent::~RenderComponent()
{
    delete pModel;
}
