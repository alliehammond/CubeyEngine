#pragma once

#include "Graphics\Mesh.h"
#include "d3d11.h"
#include <vector>
#include <string>

class Model
{
public:
    //Meshes create a duplicate of mat
    Model(std::string fileName, Material *mat, InputLayout IL);
    //Creates a model with no meshes
    Model();
    ~Model();
    //Deletes loaded model if one exists; returns true if successfully loaded model - meshes create a duplicate of mat
    bool LoadModel(std::string fileName, Material *mat, InputLayout IL);
    void AddMesh(Mesh *pMesh);
    void ClearModel();
private:
    std::vector<Mesh *> meshes;

    friend class GraphicsSystem;
};
