#pragma once

class Material
{
public:
    Material(std::string VS, std::string PS, InputLayout IL, std::string name_);
    virtual ~Material() {}
    virtual void BindMaterial();
    virtual Material* Clone();
    std::string GetName() { return name; }
    unsigned int GetVertexSize() { return vertexSize; }
    InputLayout GetInputLayout() { return ILayout; }
private:
    ID3D11PixelShader *pPixShader = 0;
    ID3D11VertexShader *pVertShader = 0;
    ID3D11InputLayout *pInputLayout = 0;
    std::string name = "Material";
    unsigned int vertexSize;
    InputLayout ILayout;
protected:
    Material();
};
