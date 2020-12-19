#include "EnginePCH.h"

using namespace DirectX;

const int _windowWidth = 1280;
const int _windowHeight = 720;
LPCSTR _windowClassName = "CubeyEngineWindowClass";
LPCSTR _windowName = "Cubey Engine!";
HWND _windowHandle = 0;

const bool _enableVSync = false;

// Direct3D device and swap chain.
ID3D11Device* _d3dDevice = nullptr;
ID3D11DeviceContext* _d3dDeviceContext = nullptr;
IDXGISwapChain* _d3dSwapChain = nullptr;
// Render target view for the back buffer of the swap chain.
ID3D11RenderTargetView* _d3dRenderTargetView = nullptr;
// Depth/stencil view for use as a depth buffer.
ID3D11DepthStencilView* _d3dDepthStencilView = nullptr;
// A texture to associate to the depth stencil view.
ID3D11Texture2D* _d3dDepthStencilBuffer = nullptr;
// Define the functionality of the depth/stencil stages.
ID3D11DepthStencilState* _d3dDepthStencilState = nullptr;
// Define the functionality of the rasterizer stage.
ID3D11RasterizerState* _d3dRasterizerState = nullptr;
D3D11_VIEWPORT _viewport = { 0 };

//TEMP SHADER STUFF
// Vertex buffer data
ID3D11InputLayout* g_d3dInputLayout = nullptr;
ID3D11Buffer* g_d3dVertexBuffer = nullptr;
ID3D11Buffer* g_d3dIndexBuffer = nullptr;

// Shader data
ID3D11VertexShader* g_d3dVertexShader = nullptr;
ID3D11PixelShader* g_d3dPixelShader = nullptr;

// Shader resources
//3 Different constant buffers - updated rarely, per frame, and per object - reduces how much data needs to be rewritten
enum ConstantBuffer
{
    CB_Application,
    CB_Frame,
    CB_Object,
    NumConstantBuffers
};

ID3D11Buffer* _d3dConstantBuffers[NumConstantBuffers];

int main()
{

    return 0;
}
