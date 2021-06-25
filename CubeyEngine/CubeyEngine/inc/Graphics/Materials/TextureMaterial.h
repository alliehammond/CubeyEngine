#pragma once
#include "Graphics\Material.h"
#include "Graphics\Texture.h"

class TextureMaterial : public Material
{
public:
    TextureMaterial(std::string VS, std::string PS, InputLayout IL, std::string name_, std::string texName);
    ~TextureMaterial() {}
    void BindMaterial();
    Material *Clone();
private:
    TextureMaterial();
    Texture *texture;
};
