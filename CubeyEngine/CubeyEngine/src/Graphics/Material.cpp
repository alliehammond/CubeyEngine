#include "EnginePCH.h"
#include "Graphics\Material.h"

Material* Material::Clone()
{
    Material *mat = new Material;
    (*mat) = (*this);
    return mat;
}
