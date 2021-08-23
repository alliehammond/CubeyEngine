#pragma once
#include "Graphics\Material.h"
#include "Graphics\Texture.h"

struct ConstantBufferBlockIconMaterial
{
    //X position, Y position, width, height of block icon ui element
    //Width, height as percentage of screen size
    //Position between -1 and 1
    XMFLOAT4 posSize;
    //First float - block type, second float - num blocks, third float - bool true if block icon selected
    XMFLOAT4 blockTypeNumBlocks;
};

class BlockIconMaterial : public Material
{
public:
    BlockIconMaterial(std::string VS, std::string PS, InputLayout IL, std::string name_, std::string texName, BlockType bType_, float posX_, float posY_, float width_, float height_);
    ~BlockIconMaterial() {}
    void BindConstantBuffer(DirectX::XMMATRIX *worldMatrix, DirectX::XMMATRIX *rotationMatrix);
    void BindMaterial();
    Material *Clone();

    BlockType bType;
    float posX, posY, width, height;
    bool blockIconSelected = false;
private:
    BlockIconMaterial();
    Texture *texture;

    static ID3D11Buffer *constantBufferBlockIconMaterial;

    static ConstantBufferBlockIconMaterial cBufStruct;

    friend class GraphicsSystem;
};
