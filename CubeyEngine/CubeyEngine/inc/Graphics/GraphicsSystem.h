#pragma once
#include "Core\CubeySystem.h"
#include "Graphics\Mesh.h"
#include "d3d11.h"
#include <DirectXMath.h>
#include <string>
#include <unordered_map>

enum InputLayout
{
    POSCOL,
    INPUTLAYOUTCOUNT
};

using DirectX::XMMATRIX;
using DirectX::XMFLOAT3;

//Manages creation and deletion of objects
class GraphicsSystem : public CubeySystem
{
public:
    GraphicsSystem(HINSTANCE hInstance, int cmdShow);
    ~GraphicsSystem();
    void Update(float dt);

    static ID3D11PixelShader *GetPixelShader(std::string name) { return pixelShaders[name]; }
    static ID3D11VertexShader* GetVertexShader(std::string name) { return vertexShaders[name]; }
    static ID3D11InputLayout* GetInputLayout(InputLayout layout) { return inputLayouts[layout]; }
    static ID3D11Device *GetD3DDevice() { return d3dDevice; }

    static void ResizeWindow(int width, int height);


    //*****************************************
    //Input Layouts
    //*****************************************
    struct VertexPosColor
    {
        XMFLOAT3 position;
        XMFLOAT3 color;
    };
    //*****************************************

private:
    void LoadPixelShader(std::string fileName, std::wstring fileNameWide);
    void LoadVertexShader(std::string fileName, std::wstring fileNameWide);
    void LoadInputLayouts();
    void CreateConstantBuffers();

    void InitApplication(HINSTANCE hInstance, int cmdShow);
    void InitDirectX(HINSTANCE hInstance);
    void Clear(const FLOAT clearColor[4], FLOAT clearDepth, UINT8 clearStencil);
    void Render(float dt);
    void RenderObject(Mesh *pMesh, float dt);

    static std::unordered_map<std::string, ID3D11VertexShader *> vertexShaders;
    static std::unordered_map<std::string, ID3D11PixelShader*> pixelShaders;
    static std::unordered_map<InputLayout, ID3D11InputLayout *> inputLayouts;

    //*****************************************
    //Config Values
    //*****************************************
    static int windowWidth;
    static int windowHeight;
    LPCWSTR windowClassNameW = L"CubeyEngineWindowClass";
    LPCWSTR windowNameW = L"Cubey Engine!";
    LPCSTR windowClassName = "CubeyEngineWindowClass";
    LPCSTR windowName = "Cubey Engine!";
    const bool enableVSync = false;
    //*****************************************

    static HWND windowHandle;
    static ID3D11Device* d3dDevice;
    static ID3D11DeviceContext* d3dDeviceContext;
    static IDXGISwapChain* d3dSwapChain;
    static ID3D11RenderTargetView* d3dRenderTargetView;
    static ID3D11DepthStencilView* d3dDepthStencilView;
    static ID3D11Texture2D* d3dDepthStencilBuffer;
    static ID3D11DepthStencilState* d3dDepthStencilState;
    static ID3D11RasterizerState* d3dRasterizerState;
    static D3D11_VIEWPORT viewport;

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
};
