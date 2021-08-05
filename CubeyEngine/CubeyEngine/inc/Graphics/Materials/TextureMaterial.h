#pragma once
#include "Graphics\Material.h"
#include "Graphics\Texture.h"

struct ConstantBufferTextureMaterial
{
    XMMATRIX worldMatrix;
    XMMATRIX rotationMatrix;
    XMFLOAT4 ks;
    XMFLOAT4 shininess;
};

class TextureMaterial : public Material
{
public:
    TextureMaterial(std::string VS, std::string PS, InputLayout IL, std::string name_, std::string texName, CBY::Vector specularReflection_ = CBY::Vector(0.1f, 0.1f, 0.1f), float shininess_ = 1.0f);
    ~TextureMaterial() {}
    void BindConstantBuffer(DirectX::XMMATRIX *worldMatrix, DirectX::XMMATRIX *rotationMatrix);
    void BindMaterial();
    Material *Clone();

    CBY::Vector specularReflection;
    float shininess;
private:
    TextureMaterial();
    Texture *texture;

    static ID3D11Buffer *constantBufferTextureMaterial;

    static ConstantBufferTextureMaterial cBufStruct;

    friend class GraphicsSystem;
};
