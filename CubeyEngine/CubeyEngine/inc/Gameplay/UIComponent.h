#pragma once

#include "Core\Component.h"
#include "Graphics/Materials/BlockIconMaterial.h"

class UIComponent : public Component
{
public:
    UIComponent(GameObject *owner);
    ~UIComponent();
    void Update(float dt);
    void PostUpdate(float dt);

    void SetBlockIconType(unsigned pos, BlockType newBlockType);
    BlockType GetBlockIconType(unsigned index);

private:
    std::vector<BlockType> blockIconTypes;
    std::vector<GameObject *> blockIconObjects;
    std::vector<BlockIconMaterial *> blockIconMaterials;
};
