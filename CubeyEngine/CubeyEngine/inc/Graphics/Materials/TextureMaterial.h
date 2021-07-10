#pragma once
#include "Graphics\Material.h"
#include "Graphics\Texture.h"

#include <memory>

class TextureMaterial : public Material
{
public:
    TextureMaterial(std::string VS, std::string PS, InputLayout IL, std::string name_, std::string texName);
    ~TextureMaterial() {}
    void BindMaterial();
    std::unique_ptr<Material> Clone();
private:
    TextureMaterial();
    Texture *texture;
};
