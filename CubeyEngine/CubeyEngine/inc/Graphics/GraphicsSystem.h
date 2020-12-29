#pragma once
#include "Core\CubeySystem.h"
#include "d3d11.h"
#include <string>
#include <unordered_map>

struct Material
{
    ID3D11PixelShader *pPixShader = 0;
    ID3D11VertexShader *pVertShader = 0;
    std::string name = "Material";
};

enum InputLayout
{
    POSCOL,
    INPUTLAYOUTCOUNT
};

//Manages creation and deletion of objects
class GraphicsSystem : public CubeySystem
{
public:
    GraphicsSystem(HINSTANCE hInstance, int cmdShow);
    ~GraphicsSystem();
    void Update(float dt);

private:
    void LoadPixelShader(std::string fileName, std::wstring fileNameWide);
    void LoadVertexShader(std::string fileName, std::wstring fileNameWide);
    void LoadInputLayouts();
    void CreateConstantBuffers();

    void InitApplication(HINSTANCE hInstance, int cmdShow);
    void InitDirectX(HINSTANCE hInstance);

    std::unordered_map<std::string, ID3D11VertexShader *> vertexShaders;
    std::unordered_map<std::string, ID3D11PixelShader*> pixelShaders;
    std::unordered_map<InputLayout, ID3D11InputLayout *> inputLayouts;

    //*****************************************
    //Config Values
    //*****************************************
    const float frameRateCap = 120.0f;
    const int windowWidth = 800;
    const int windowHeight = 450;
    LPCWSTR windowClassNameW = L"CubeyEngineWindowClass";
    LPCWSTR windowNameW = L"Cubey Engine!";
    LPCSTR windowClassName = "CubeyEngineWindowClass";
    LPCSTR windowName = "Cubey Engine!";
    const bool enableVSync = false;
    //*****************************************

    HWND windowHandle = 0;
    ID3D11Device* d3dDevice = 0;
    ID3D11DeviceContext* d3dDeviceContext = 0;
    IDXGISwapChain* d3dSwapChain = 0;
    ID3D11RenderTargetView* d3dRenderTargetView = 0;
    ID3D11DepthStencilView* d3dDepthStencilView = 0;
    ID3D11Texture2D* d3dDepthStencilBuffer = 0;
    ID3D11DepthStencilState* d3dDepthStencilState = 0;
    ID3D11RasterizerState* d3dRasterizerState = 0;
    D3D11_VIEWPORT viewport = { 0 };

    //Temp
    ID3D11Buffer *d3dVertexBuffer = 0, *d3dIndexBuffer = 0;

    //3 Different constant buffers - updated rarely, per frame, and per object - reduces how much data needs to be rewritten
    enum ConstantBuffer
    {
        CB_Application,
        CB_Frame,
        CB_Object,
        NumConstantBuffers
    };
    ID3D11Buffer* d3dConstantBuffers[NumConstantBuffers];

    XMMATRIX worldMatrix, viewMatrix, projectionMatrix;

    //*****************************************
    //Input Layouts
    //*****************************************
    struct VertexPosColor
    {
        XMFLOAT3 position;
        XMFLOAT3 color;
    };
    //*****************************************

    //Temp Cube
    VertexPosColor _cubeVertices[8] =
    {
        { XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f) }, //Each cube face consists of 2 triangles
        { XMFLOAT3(-1.0f,  1.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) },
        { XMFLOAT3(1.0f,  1.0f, -1.0f), XMFLOAT3(1.0f, 1.0f, 0.0f) },
        { XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f) },
        { XMFLOAT3(-1.0f, -1.0f,  1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
        { XMFLOAT3(-1.0f,  1.0f,  1.0f), XMFLOAT3(0.0f, 1.0f, 1.0f) },
        { XMFLOAT3(1.0f,  1.0f,  1.0f), XMFLOAT3(1.0f, 1.0f, 1.0f) },
        { XMFLOAT3(1.0f, -1.0f,  1.0f), XMFLOAT3(1.0f, 0.0f, 1.0f) }
    };

    WORD _indices[36] =
    {
        0, 1, 2, 0, 2, 3,
        4, 6, 5, 4, 7, 6,
        4, 5, 1, 4, 1, 0,
        3, 2, 6, 3, 6, 7,
        1, 5, 6, 1, 6, 2,
        4, 0, 3, 4, 3, 7
    };
};
