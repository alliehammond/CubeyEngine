#pragma once
#include "d3d11.h"
#include <string>


struct Material
{
    ID3D11PixelShader *pPixShader = 0;
    ID3D11VertexShader *pVertShader = 0;
    ID3D11InputLayout *pInputLayout = 0;
    std::string name = "Material";

    virtual Material *Clone();
};
