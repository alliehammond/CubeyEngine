#pragma once

#include "Graphics\Mesh.h"
#include "d3d11.h"
#include <vector>
#include <string>

class Model
{
public:
    Model(std::string fileName, Material mat);
    ~Model();
    //Deletes loaded model if one exists; returns true if successfully loaded model
    bool LoadModel(std::string fileName, Material mat);
private:
    std::vector<Mesh *> meshes;

    friend class GraphicsSystem;
};
