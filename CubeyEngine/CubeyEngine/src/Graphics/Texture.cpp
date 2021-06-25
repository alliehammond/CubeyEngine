#include "EnginePCH.h"
#include "Graphics\Texture.h"
#include "DirectXTex.h"

#include <wrl.h>

const std::string texturePath = "../resources/textures/";

const wchar_t* GetWC(const char* c)
{
    const size_t cSize = strlen(c) + 1;
    wchar_t* wc = new wchar_t[cSize];
    mbstowcs(wc, c, cSize);

    return wc;
}

Texture::Texture(std::string fileName) : textureName(fileName), texture(0), shaderResourceView(0)
{
    DirectX::TexMetadata metadata;
    DirectX::ScratchImage image;

    std::string texPath = texturePath + fileName;
    const wchar_t* name = GetWC(texPath.c_str());

    HRESULT hr = DirectX::LoadFromTGAFile(name, &metadata, image);
    if(FAILED(hr))
    {
        LOGWARNING("Unable to load texture! " + textureName);
        return;
    }
    delete[] name;

    D3D11_TEXTURE2D_DESC desc;
    D3D11_SUBRESOURCE_DATA data;

    desc.Width = (unsigned int)metadata.width;
    desc.Height = (unsigned int)metadata.height;
    desc.MipLevels = (unsigned int)metadata.mipLevels;
    desc.ArraySize = (unsigned int)metadata.arraySize;
    desc.Format = metadata.format;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.Usage = D3D11_USAGE_IMMUTABLE;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = 0;

    data.pSysMem = image.GetImages()->pixels;
    data.SysMemPitch = (unsigned int)image.GetImages()->rowPitch;
    data.SysMemSlicePitch = (unsigned int)image.GetImages()->slicePitch;

    hr = GraphicsSystem::GetD3DDevice()->CreateTexture2D(&desc, &data, &texture);
    if(FAILED(hr))
    {
        LOGWARNING("Failed to create texture! " + textureName);
        return;
    }

    D3D11_SHADER_RESOURCE_VIEW_DESC shaderDesc;
    shaderDesc.Format = metadata.format;
    shaderDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    shaderDesc.Texture2D.MipLevels = 1;
    shaderDesc.Texture2D.MostDetailedMip = 0;

    hr = GraphicsSystem::GetD3DDevice()->CreateShaderResourceView(texture, &shaderDesc, &shaderResourceView);
    if(FAILED(hr))
    {
        LOGWARNING("Failed to create texture!(shader resource view) " + textureName);
        return;
    }
}

Texture::~Texture()
{
    SafeRelease(texture);
    SafeRelease(shaderResourceView);
}

void Texture::BindAsTexture()
{
    GraphicsSystem::GetD3DDeviceContext()->PSSetShaderResources(0, 1, &shaderResourceView);
}
