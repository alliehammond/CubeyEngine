#pragma once

#include <d3d11.h>

class Texture
{
public:
    Texture(std::string fileName);
    ~Texture();

    void BindAsTexture();

    std::string textureName;
    ID3D11Texture2D* texture;
    ID3D11ShaderResourceView* shaderResourceView;
};
