#include "EnginePCH.h"
#include "Gameplay/UIComponent.h"

UIComponent::UIComponent(GameObject *owner) : Component(owner, "UIComponent")
{
    //Create render component with UI texture
    //owner->AddComponent<RenderComponent>(new RenderComponent(
}

UIComponent::~UIComponent()
{ }

void UIComponent::Update(float dt)
{

}
