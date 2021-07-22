#include "EnginePCH.h"
#include "Gameplay/UIComponent.h"
#include "Graphics\Materials\TextureMaterial.h"

UIComponent::UIComponent(GameObject *owner) : Component(owner, "UIComponent")
{
    TextureMaterial mat("UITextureVS.cso", "BasicTexturePS.cso", InputLayout::POSUV, "UITextureMaterial", "UITexture.tga");
    //Create render component with UI texture
    RenderComponent *rComp = owner->AddComponent<RenderComponent>(new RenderComponent("VerticalPlane.fbx", &mat, owner));
    rComp->transparent = true;
}

UIComponent::~UIComponent()
{ }

void UIComponent::PostUpdate(float dt)
{
    //Update object to player position to ensure it is rendered last
    Transform *plPos = PlayerController::GetPlayerTrans();
    if(plPos)owner->GetComponent<Transform>()->pos = plPos->pos;
}
