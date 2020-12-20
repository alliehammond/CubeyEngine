#include "EnginePCH.h"

using namespace DirectX;

const int _windowWidth = 1280;
const int _windowHeight = 720;
LPCWSTR _windowClassNameW = L"CubeyEngineWindowClass";
LPCWSTR _windowNameW = L"Cubey Engine!";
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
ID3D11InputLayout* _d3dInputLayout = nullptr;
ID3D11Buffer* _d3dVertexBuffer = nullptr;
ID3D11Buffer* _d3dIndexBuffer = nullptr;

// Shader data
ID3D11VertexShader* _d3dVertexShader = nullptr;
ID3D11PixelShader* _d3dPixelShader = nullptr;

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

XMMATRIX _worldMatrix, _viewMatrix, _projectionMatrix;

struct VertexPosColor
{
    XMFLOAT3 position;
    XMFLOAT3 color;
};

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

//Windows message handler
LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
//Main loop
int Run();

template<class ShaderClass>
ShaderClass* LoadShader(const std::wstring& fileName, const std::string& entryPoint, const std::string& profile);

//Load resources to gpu (temp probs)
bool LoadContent();
void UnloadContent();

void Update(float deltaTime);
void Render();
void Cleanup();

//Init windows application
int InitApplication(HINSTANCE hInstance, int cmdShow)
{
    WNDCLASSEX wndClass = { 0 };
    wndClass.cbSize = sizeof(WNDCLASSEX);
    wndClass.style = CS_HREDRAW | CS_VREDRAW; //Redraw window on resize (horizontal/vertical)
    wndClass.lpfnWndProc = &WndProc;
    wndClass.hInstance = hInstance;
    wndClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wndClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wndClass.lpszMenuName = nullptr;
    wndClass.lpszClassName = _windowClassNameW;

    if (!RegisterClassEx(&wndClass))
    {
        MessageBox(nullptr, TEXT("Could not register window class!"), TEXT("Error"), MB_OK);
        return -1;
    }

    RECT windowRect = { 0, 0, _windowWidth, _windowHeight };
    AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

    _windowHandle = CreateWindowA(_windowClassName, _windowName,
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
        windowRect.right - windowRect.left,
        windowRect.bottom - windowRect.top,
        nullptr, nullptr, hInstance, nullptr);

    if (!_windowHandle)
    {
        MessageBox(nullptr, TEXT("Could not create window!"), TEXT("Error"), MB_OK);
        return -1;
    }

    ShowWindow(_windowHandle, cmdShow);
    UpdateWindow(_windowHandle);

    return 0;
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE prevInstance, LPWSTR cmdLine, int cmdShow)
{
    UNREFERENCED_PARAMETER(prevInstance);
    UNREFERENCED_PARAMETER(cmdLine);

    // Check for DirectX Math library support.
    if (!XMVerifyCPUSupport())
    {
        MessageBox(nullptr, TEXT("Failed to verify DirectX Math library support."), TEXT("Error"), MB_OK);
        return -1;
    }

    if (InitApplication(hInstance, cmdShow) != 0)
    {
        MessageBox(nullptr, TEXT("Failed to create applicaiton window."), TEXT("Error"), MB_OK);
        return -1;
    }

    int returnCode = Run();

    return returnCode;
}


LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT paintStruct;
    HDC hDC;

    switch (message)
    {
            case WM_PAINT:
        {
            hDC = BeginPaint(hwnd, &paintStruct);
            EndPaint(hwnd, &paintStruct);
        }
        break;
            case WM_DESTROY:
        {
            PostQuitMessage(0);
        }
        break;
            default:
        return DefWindowProc(hwnd, message, wParam, lParam);
        }

    return 0;
}

//Main application loop
int Run()
{
    MSG msg = { 0 };

    static DWORD previousTime = timeGetTime();

    while (msg.message != WM_QUIT)
    {
        if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            DWORD currentTime = timeGetTime();
            float deltaTime = (currentTime - previousTime) / 1000.0f;
            previousTime = currentTime;


            //            Update( deltaTime );
            //            Render();
        }
    }

    return static_cast<int>(msg.wParam);
}
