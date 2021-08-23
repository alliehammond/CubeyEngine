#pragma once
#include "Graphics\Material.h"
#include "Graphics\Texture.h"

struct ConstantBufferUIElementMaterial
{
    //X position, Y position, width, height of ui element
    //Width, height as percentage of screen size
    //Position between -1 and 1
    XMFLOAT4 posSize;
};

class UIElementMaterial : public Material
{
public:
    UIElementMaterial(std::string VS, std::string PS, InputLayout IL, std::string name_, std::string texName, float posX_, float posY_, float width_, float height_);
    ~UIElementMaterial() {}
    void BindConstantBuffer(DirectX::XMMATRIX *worldMatrix, DirectX::XMMATRIX *rotationMatrix);
    void BindMaterial();
    Material *Clone();

    float posX, posY, width, height;
private:
    UIElementMaterial();
    Texture *texture;

    static ID3D11Buffer *constantBufferUIElementMaterial;

    static ConstantBufferUIElementMaterial cBufStruct;

    friend class GraphicsSystem;
};
