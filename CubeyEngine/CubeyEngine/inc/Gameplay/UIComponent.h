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

    static void SetBlockIconType(unsigned pos, BlockType newBlockType);
    static BlockType GetBlockIconType(unsigned index);

private:
    static std::vector<BlockType> blockIconTypes;
    static std::vector<GameObject *> blockIconObjects;
    static std::vector<BlockIconMaterial *> blockIconMaterials;

    static GameObject *blockIconSelectionObject;
};
