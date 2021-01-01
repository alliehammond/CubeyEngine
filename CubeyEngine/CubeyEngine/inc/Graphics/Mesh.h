#pragma once
#include "Graphics/Material.h"
#include "d3d11.h"
#include <string>

class Mesh
{
public:
    //Loads firsst mesh in file (designed for single mesh files)
    Mesh(std::string fileName, Material mat);
    ~Mesh();
    //Returns true on success, if a mesh is already loaded deletes that first
    bool LoadMesh(std::string fileName, Material mat);

    Material material;
private:
    ID3D11Buffer *vertexBuffer = 0, *indexBuffer = 0;
};
