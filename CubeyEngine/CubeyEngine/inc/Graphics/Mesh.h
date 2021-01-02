#pragma once
#include "Graphics/Material.h"
#include "d3d11.h"
#include <string>

class Mesh
{
public:
    //Loads firsst mesh in file (designed for single mesh files)
    Mesh();
    ~Mesh();

    Material material;
private:
    ID3D11Buffer *vertexBuffer = 0, *indexBuffer = 0;
    friend class Model;
    friend class GraphicsSystem;
};
