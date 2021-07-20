#pragma once
#include "Core\CubeySystem.h"
#include "d3d11.h"
#include <DirectXMath.h>
#include <string>
#include <unordered_map>


using DirectX::XMMATRIX;
using DirectX::XMFLOAT2;
using DirectX::XMFLOAT3;
using DirectX::XMVECTOR;

//*****************************************
//Input Layouts
//*****************************************
struct VertexPosColor
{
    XMFLOAT3 position;
    XMFLOAT3 color;
};
struct VertexPosUV
{
    XMFLOAT3 position;
    XMFLOAT2 uv;
};
//*****************************************
enum InputLayout
{
    POSCOL = 0,
    POSUV,
    INPUTLAYOUTCOUNT
};
const unsigned int InputLayoutVertexSizes[InputLayout::INPUTLAYOUTCOUNT] = { sizeof(VertexPosColor), sizeof(VertexPosUV) };

class Texture;

//Manages creation and deletion of objects
class GraphicsSystem : public CubeySystem
{
public:
    GraphicsSystem(HINSTANCE hInstance, int cmdShow);
    ~GraphicsSystem();
    void Update(float dt);

    static ID3D11PixelShader *GetPixelShader(std::string name);
    static ID3D11VertexShader* GetVertexShader(std::string name);
    static ID3D11InputLayout* GetInputLayout(InputLayout layout) { return inputLayouts[layout]; }
    //Gets a texture from texture pool, or loads it if it isn't already loaded - returns null if texture couldn't be loaded
    static Texture *GetTexture(std::string textureName);
    static ID3D11Device *GetD3DDevice() { return d3dDevice; }
    static ID3D11DeviceContext* GetD3DDeviceContext() { return d3dDeviceContext; }

    static void ResizeWindow(int width, int height);

    static bool GetMouseCursorLock() { return lockMouseCenter; }
    static int GetWindowWidth() { return windowWidth; }
    static int GetWindowHeight() { return windowHeight; }

private:
    static void SetCameraTrans(Transform *trans);

    static ID3D11PixelShader *LoadPixelShader(std::string fileName, std::wstring fileNameWide);
    static ID3D11VertexShader *LoadVertexShader(std::string fileName, std::wstring fileNameWide);
    void LoadInputLayouts();
    void CreateConstantBuffers();

    void InitApplication(HINSTANCE hInstance, int cmdShow);
    void InitDirectX(HINSTANCE hInstance);
    void Clear(const FLOAT clearColor[4], FLOAT clearDepth, UINT8 clearStencil);
    void Render(float dt);
    void RenderObject(GameObject *pObject, float dt);

    static std::unordered_map<std::string, ID3D11VertexShader *> vertexShaders;
    static std::unordered_map<std::string, ID3D11PixelShader*> pixelShaders;
    static std::unordered_map<InputLayout, ID3D11InputLayout *> inputLayouts;
    static std::unordered_map<std::string, Texture*> textures;

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
    static ID3D11BlendState* d3dBlendState;
    static ID3D11SamplerState* d3dSamplerState;
    static D3D11_VIEWPORT viewport;

    static XMVECTOR eyePosition;
    static XMVECTOR focusPoint;

    //Locks the mouse cursor to center of screen
    static bool lockMouseCenter;
    static bool windowMinimized;

    //3 Different constant buffers - updated rarely, per frame, and per object - reduces how much data needs to be rewritten
    enum ConstantBuffer
    {
        CB_Application,
        CB_Frame,
        CB_Object,
        NumConstantBuffers
    };
    static ID3D11Buffer* d3dConstantBuffers[NumConstantBuffers];

    static XMMATRIX viewMatrix, projectionMatrix;

    friend class PlayerController;
};
