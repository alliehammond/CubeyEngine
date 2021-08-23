#include "EnginePCH.h"

#include "Terrain\BlockType.h"
#include "Graphics\Materials\TextureMaterial.h"
#include "Graphics\GraphicsSystem.h"
#include "Core\LoggingSystem.h"

namespace CBY
{

    //Pointer that is returned must be manually deleted
    Material *GetBlockMaterial(BlockType bType)
    {
        switch(bType)
        {
            case BlockType::Dirt:
            {
                return new TextureMaterial("BasicTextureVS.cso", "BasicTexturePS.cso", InputLayout::POSUVNORM, "BaseTextureMaterial", "dirtTexture.tga");
            }
            case BlockType::Grass:
            {
                return new TextureMaterial("BasicTextureTopSideBottomVS.cso", "BasicTextureTopSideBottomPS.cso", InputLayout::POSUVNORM, "BaseTextureMaterial", "grassTexture.tga");
            }
            case BlockType::Stone:
            {
                return new TextureMaterial("BasicTextureVS.cso", "BasicTexturePS.cso", InputLayout::POSUVNORM, "BaseTextureMaterial", "stoneTexture.tga");
            }
            case BlockType::Log:
            {
                return new TextureMaterial("BasicTextureTopSideBottomVS.cso", "BasicTextureTopSideBottomPS.cso", InputLayout::POSUVNORM, "BaseTextureMaterial", "logTexture.tga");
            }
            case BlockType::Leaf:
            {
                return new TextureMaterial("BasicTextureVS.cso", "BasicTextureTransPS.cso", InputLayout::POSUVNORM, "BaseTextureMaterial", "leafTexture.tga");
            }
            case BlockType::Plank:
            {
                return new TextureMaterial("BasicTextureVS.cso", "BasicTexturePS.cso", InputLayout::POSUVNORM, "BaseTextureMaterial", "plankTexture.tga");
            }
            case BlockType::Glass:
            {
                return new TextureMaterial("BasicTextureVS.cso", "BasicTextureTransPS.cso", InputLayout::POSUVNORM, "BaseTextureMaterial", "glassTexture.tga");
            }
            case BlockType::FlowerLeaf:
            {
                return new TextureMaterial("BasicTextureVS.cso", "BasicTextureTransPS.cso", InputLayout::POSUVNORM, "BaseTextureMaterial", "flowerLeafTexture.tga");
            }
            default:
                LOGERROR("Invalid block type attempting to be loaded!");
        }

        return 0;
    }

};