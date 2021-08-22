#include "EnginePCH.h"
#include "Gameplay/UIComponent.h"
#include "Graphics\Materials\TextureMaterial.h"
#include "Graphics\Materials\BlockIconMaterial.h"

UIComponent::UIComponent(GameObject *owner) : Component(owner, "UIComponent")
{
    TextureMaterial mat("UITextureVS.cso", "BasicTextureUIPS.cso", InputLayout::POSUV, "UITextureMaterial", "UITexture.tga");
    //Create render component with UI texture
    RenderComponent *rComp = owner->AddComponent<RenderComponent>(new RenderComponent("VerticalPlane.fbx", &mat, owner));
    rComp->transparent = true;
    
    
    //Create block icons
    for(int i = 0; i < 10; ++i)
    {
        blockIconTypes.push_back(BlockType::Air);

        //The floats are the position/width/height of each block icon
        BlockIconMaterial blockMat("UIBlockIconVS.cso", "UIBlockIconPS.cso", InputLayout::POSUV, "BlockIconMaterial", "blockIconTexture.tga", BlockType::Air, -0.4692708f + 0.105208f * float(i), -0.8638888f, 0.041666666f, 0.0740740f);
        blockIconObjects.push_back(ObjectManagerSystem::CreateObject(new GameObject("BlockIcon")));
        RenderComponent *renderComp = blockIconObjects[i]->AddComponent<RenderComponent>(new RenderComponent("VerticalPlane.fbx", &blockMat, blockIconObjects[i]));

        //Assume its safe to cast the material to BlockIconMaterial
        blockIconMaterials.push_back((BlockIconMaterial *)renderComp->pModel->GetMesh(0)->material);
    }

    //Tempoararily hardcode block icons
    SetBlockIconType(0, BlockType::Dirt);
    SetBlockIconType(1, BlockType::Grass);
    SetBlockIconType(2, BlockType::Stone);
    SetBlockIconType(3, BlockType::Log);
    SetBlockIconType(4, BlockType::Leaf);
    SetBlockIconType(5, BlockType::Plank);
}

UIComponent::~UIComponent()
{
    for(auto &it : blockIconObjects)
    {
        it->Delete();
    }
}

void UIComponent::Update(float dt)
{
    //Toggle UI on and off
    if(InputSystem::GetKeyPressed('U'))
    {
        owner->GetComponent<RenderComponent>()->renderComponent = !owner->GetComponent<RenderComponent>()->renderComponent;
    }
}

void UIComponent::PostUpdate(float dt)
{
    //Update object to player position to ensure it is rendered last
    Transform *plPos = PlayerController::GetPlayerTrans();
    if(plPos)owner->GetComponent<Transform>()->pos = plPos->pos;
}

void UIComponent::SetBlockIconType(unsigned pos, BlockType newBlockType)
{
    //Temporarily use hardcoded block icons
    blockIconTypes[pos] = newBlockType;
    blockIconMaterials[pos]->bType = newBlockType;
}

BlockType UIComponent::GetBlockIconType(unsigned index)
{
    if(index >= blockIconTypes.size())return BlockType::BLOCKCOUNT;
    return blockIconTypes[index];
}
