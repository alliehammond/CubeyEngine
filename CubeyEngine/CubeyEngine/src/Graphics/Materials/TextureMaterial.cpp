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

Material* TextureMaterial::Clone()
{
    TextureMaterial* mat = new TextureMaterial;
    (*mat) = (*this);
    return mat;
}
