#include "EnginePCH.h"
#include "Graphics\Materials\TextureMaterial.h"

TextureMaterial::TextureMaterial() : Material(), texture(0) {}

TextureMaterial::TextureMaterial(std::string VS, std::string PS, InputLayout IL, std::string name_, std::string texName) : Material(VS, PS, IL, name_), texture(0)
{
    texture = GraphicsSystem::GetTexture(texName);
}

void TextureMaterial::BindMaterial()
{
    //Texture should always be initialized (to error texture if texture couldn't be found)
    texture->BindAsTexture();
    Material::BindMaterial();
}

std::unique_ptr<Material> TextureMaterial::Clone()
{
    std::unique_ptr<TextureMaterial> mat = std::make_unique<TextureMaterial>();
    (*mat) = (*this);
    return mat;
}
