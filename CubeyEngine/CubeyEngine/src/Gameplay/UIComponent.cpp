#include "EnginePCH.h"
#include "Gameplay/UIComponent.h"
#include "Graphics\Materials\TextureMaterial.h"
#include "Graphics\Materials\BlockIconMaterial.h"
#include "Graphics\Materials\UIElementMaterial.h"

std::vector<BlockType> UIComponent::blockIconTypes;
std::vector<GameObject *> UIComponent::blockIconObjects;
std::vector<BlockIconMaterial *> UIComponent::blockIconMaterials;
GameObject *UIComponent::blockIconSelectionObject = 0;

UIComponent::UIComponent(GameObject *owner) : Component(owner, "UIComponent")
{
    //Create UI texture
    TextureMaterial mat("UITextureVS.cso", "BasicTextureUIPS.cso", InputLayout::POSUV, "UITextureMaterial", "UITexture.tga");
    //Create render component with UI texture
    RenderComponent *rComp = owner->AddComponent<RenderComponent>(new RenderComponent("VerticalPlane.fbx", &mat, owner));
    rComp->transparent = true;
    
    //Create block selection icon object
    blockIconSelectionObject = ObjectManagerSystem::CreateObject(new GameObject("BlockIconSelection"));
    UIElementMaterial blockMat("UIElementVS.cso", "UIElementTransPS.cso", InputLayout::POSUV, "BlockIconSelectionMaterial", "inventoryGridSelect.tga", -0.3473958333f + 0.07720416666f * float(PlayerController::curSelectedBlock), -0.9268518518f, 0.0416666666f, 0.073148148148f);
    blockIconSelectionObject->AddComponent<RenderComponent>(new RenderComponent("VerticalPlane.fbx", &blockMat, blockIconSelectionObject));

    //Create block icons
    for(int i = 0; i < 10; ++i)
    {
        blockIconTypes.push_back(BlockType::Air);

        //The floats are the position/width/height of each block icon
        BlockIconMaterial blockMat("UIBlockIconVS.cso", "UIBlockIconPS.cso", InputLayout::POSUV, "BlockIconMaterial", "blockIconTexture.tga", BlockType::Air, -0.3471354166f + 0.077083333f * float(i), -0.9268518518f, 0.035677083f, 0.06296296296f);
        blockIconObjects.push_back(ObjectManagerSystem::CreateObject(new GameObject("BlockIcon")));
        RenderComponent *renderComp = blockIconObjects[i]->AddComponent<RenderComponent>(new RenderComponent("VerticalPlane.fbx", &blockMat, blockIconObjects[i]));

        //Assume its safe to cast the material to BlockIconMaterial
        blockIconMaterials.push_back((BlockIconMaterial *)renderComp->pModel->GetMesh(0)->material);
    }

    //Tempoararily hardcode block icons
    SetBlockIconType(0, BlockType::Dirt);
    SetBlockIconType(1, BlockType::Grass);
    SetBlockIconType(2, BlockType::Stone);
    SetBlockIconType(3, BlockType::Plank);
    SetBlockIconType(4, BlockType::Log);
    SetBlockIconType(5, BlockType::Leaf);
    SetBlockIconType(6, BlockType::FlowerLeaf);
    SetBlockIconType(7, BlockType::Glass);
}

UIComponent::~UIComponent()
{
    for(auto &it : blockIconObjects)
    {
        it->Delete();
    }
    blockIconSelectionObject->Delete();
}

void UIComponent::Update(float dt)
{
    //Toggle UI on and off
    if(InputSystem::GetKeyPressed('U'))
    {
        owner->GetComponent<RenderComponent>()->renderComponent = !owner->GetComponent<RenderComponent>()->renderComponent;
        for(auto &it : blockIconObjects)
        {
            it->GetComponent<RenderComponent>()->renderComponent = !it->GetComponent<RenderComponent>()->renderComponent;
        }
        blockIconSelectionObject->GetComponent<RenderComponent>()->renderComponent = !blockIconSelectionObject->GetComponent<RenderComponent>()->renderComponent;
    }

    //Update currently selected block icon
    ((UIElementMaterial *)(blockIconSelectionObject->GetComponent<RenderComponent>()->pModel->GetMesh(0)->material))->posX = -0.3473958333f + 0.07720416666f * float(PlayerController::curSelectedBlock);
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
