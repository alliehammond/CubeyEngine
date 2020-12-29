#pragma once
#include "Core\CubeySystem.h"
#include <string>
#include <unordered_map>

struct Material
{
    ID3D11PixelShader *pPixShader = 0;
    ID3D11VertexShader *pVertShader = 0;
    std::string name = "Material";
};

//Manages creation and deletion of objects
class GraphicsSystem : public CubeySystem
{
public:
    GraphicsSystem();
    ~GraphicsSystem();
    void Update(float dt);

private:
    std::unordered_map<std::string, ID3D11VertexShader *> vertexShaders;
    std::unordered_map<std::string, ID3D11PixelShader*> pixelShaders;
};
