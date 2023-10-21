//Left handed coordinate system (Y-up)
//Counter clockwise winding order

#include "EnginePCH.h"
#include "Graphics\GraphicsSystem.h"
#include "Graphics\RenderComponent.h"
#include "Graphics\Texture.h"
#include "Graphics\Materials\TextureMaterial.h"
#include "Graphics\Materials\BlockIconMaterial.h"
#include "Graphics\Materials\UIElementMaterial.h"
#include <codecvt>
#include <locale>

using namespace DirectX;

namespace GS
{
    using convert_t = std::codecvt_utf8<wchar_t>;
    std::wstring_convert<convert_t, wchar_t> strconverter;

    std::string to_string(std::wstring wstr)
    {
        return strconverter.to_bytes(wstr);
    }

    std::wstring to_wstring(std::string str)
    {
        return strconverter.from_bytes(str);
    }
}

float GraphicsSystem::fieldOfViewVertical = 60.0f;
int GraphicsSystem::windowWidth = 1152;
int GraphicsSystem::windowHeight = 648;
bool GraphicsSystem::lockMouseCenter = true;
bool GraphicsSystem::windowMinimized = false;

ID3D11Device *GraphicsSystem::d3dDevice = 0;
HWND GraphicsSystem::windowHandle = 0;
ID3D11DeviceContext* GraphicsSystem::d3dDeviceContext = 0;
IDXGISwapChain* GraphicsSystem::d3dSwapChain = 0;
ID3D11RenderTargetView* GraphicsSystem::d3dRenderTargetView = 0;
ID3D11DepthStencilView* GraphicsSystem::d3dDepthStencilView = 0;
ID3D11Texture2D* GraphicsSystem::d3dDepthStencilBuffer = 0;
ID3D11DepthStencilState* GraphicsSystem::d3dDepthStencilState = 0;
ID3D11RasterizerState* GraphicsSystem::d3dRasterizerState = 0;
ID3D11BlendState* GraphicsSystem::d3dBlendState = 0;
ID3D11SamplerState* GraphicsSystem::d3dSamplerState = 0;
D3D11_VIEWPORT GraphicsSystem::viewport = { 0 };
ID3D11Buffer* GraphicsSystem::d3dConstantBuffers[NumConstantBuffers] = {0};
XMMATRIX GraphicsSystem::viewMatrix, GraphicsSystem::projectionMatrix;

std::unordered_map<std::string, ID3D11PixelShader*> GraphicsSystem::pixelShaders;
std::unordered_map<std::string, ID3D11VertexShader*> GraphicsSystem::vertexShaders;
std::unordered_map<InputLayout, ID3D11InputLayout*> GraphicsSystem::inputLayouts;
std::unordered_map<std::string, Texture*> GraphicsSystem::textures;

XMVECTOR GraphicsSystem::eyePosition = XMVectorSet(0, 0, -10, 1);
XMVECTOR GraphicsSystem::focusPoint = XMVectorSet(0, 0, 0, 1);

GraphicsSystem::GraphicsSystem(HINSTANCE hInstance, int cmdShow)
{
    //Start cursor as hidden
    ShowCursor(false);

    LOGDEBUG("Graphics system initializing...");

    InitApplication(hInstance, cmdShow);
    InitDirectX(hInstance);
    
    CreateConstantBuffers();

    LOGDEBUG("Loading input layouts...");
    LoadInputLayouts();

    // Setup the projection matrix
    RECT clientRect = {0, 0, 0, 0};
    if(windowHandle)GetClientRect(windowHandle, &clientRect);

    // Compute the exact client dimensions
    // This is required for a correct projection matrix
    float clientWidth = static_cast<float>(clientRect.right - clientRect.left);
    float clientHeight = static_cast<float>(clientRect.bottom - clientRect.top);

    projectionMatrix = XMMatrixPerspectiveFovLH(XMConvertToRadians(fieldOfViewVertical), clientWidth / clientHeight, 0.1f, 1000000000.0f);

    d3dDeviceContext->UpdateSubresource(d3dConstantBuffers[CB_Application], 0, nullptr, &projectionMatrix, 0, 0);


    LOGDEBUG("Graphics system initialized!");
}

GraphicsSystem::~GraphicsSystem()
{
    SafeRelease(d3dConstantBuffers[CB_Application]);
    SafeRelease(d3dConstantBuffers[CB_Frame]);
    
    //Release per material constant buffers
    SafeRelease(Material::constantBufferMaterial);
    SafeRelease(TextureMaterial::constantBufferTextureMaterial);
    SafeRelease(BlockIconMaterial::constantBufferBlockIconMaterial);
    SafeRelease(UIElementMaterial::constantBufferUIElementMaterial);

    for(auto &it : vertexShaders)
    {
        SafeRelease(it.second);
    }
    for(auto& it : pixelShaders)
    {
        SafeRelease(it.second);
    }
    for(auto& it : inputLayouts)
    {
        SafeRelease(it.second);
    }
    for(auto& it : textures)
    {
        delete it.second;
    }
    vertexShaders.clear();
    pixelShaders.clear();
    inputLayouts.clear();
    

    SafeRelease(d3dDepthStencilView);
    SafeRelease(d3dRenderTargetView);
    SafeRelease(d3dDepthStencilBuffer);
    SafeRelease(d3dDepthStencilState);
    SafeRelease(d3dRasterizerState);
    SafeRelease(d3dSwapChain);
    SafeRelease(d3dBlendState);
    SafeRelease(d3dSamplerState);


    d3dDeviceContext->ClearState();
    d3dDeviceContext->Flush();
    SafeRelease(d3dDeviceContext);

#if _DEBUG
    ID3D11Debug *d3dDebug;
    HRESULT hr = d3dDevice->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void **>(&d3dDebug));
    if(SUCCEEDED(hr))
    {
        hr = d3dDebug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
    }
    SafeRelease(d3dDebug);
#endif
    SafeRelease(d3dDevice);
}

//Gets a texture from texture pool, or loads it if it isn't already loaded - returns null if the texture couldn't be loaded
Texture* GraphicsSystem::GetTexture(std::string textureName)
{
    auto it = textures.find(textureName);
    if(it != textures.end())
    {
        //Texture found
        return it->second;
    }
    //Texture not found, create/add it
    Texture *pTex = new Texture(textureName);
    //Check if texture loaded correctly
    if(pTex->texture == 0)
    {
        delete pTex;
        if(textureName == "errorTexture.tga")
        {
            LOGERROR("Could not load error texture!");
            return 0;
        }
        Texture *errTex = GetTexture("errorTexture.tga");
        return errTex;
    }

    textures[textureName] = pTex;
    return pTex;
}

void GraphicsSystem::ResizeWindow(int width, int height)
{
    if(d3dSwapChain)
    {
        d3dDeviceContext->OMSetRenderTargets(0, 0, 0);
        // Release all outstanding references to the swap chain's buffers.
        SafeRelease(d3dRenderTargetView);
        SafeRelease(d3dDepthStencilView);
        SafeRelease(d3dDepthStencilBuffer);
        d3dDeviceContext->Flush();

        //Check if window minimized (w/h != 0)
        if(width && height)
        {
            windowMinimized = false;
            // Preserve the existing buffer count and format.
            d3dSwapChain->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0);

            // Get buffer and create a render-target-view.
            ID3D11Texture2D* pBuffer = 0;
            d3dSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBuffer);
            if(pBuffer)
            {
                d3dDevice->CreateRenderTargetView(pBuffer, NULL, &d3dRenderTargetView);
            }

            pBuffer->Release();

            // Set up the viewport.
            viewport.Width = float(width);
            viewport.Height = float(height);
            viewport.MinDepth = 0.0f;
            viewport.MaxDepth = 1.0f;
            viewport.TopLeftX = 0;
            viewport.TopLeftY = 0;
            d3dDeviceContext->RSSetViewports(1, &viewport);
            windowWidth = width;
            windowHeight = height;

            //Set projection matrix
            RECT clientRect = { 0, 0, 0, 0 };
            if(windowHandle)GetClientRect(windowHandle, &clientRect);

            // Compute the exact client dimensions
            // This is required for a correct projection matrix
            float clientWidth = static_cast<float>(clientRect.right - clientRect.left);
            float clientHeight = static_cast<float>(clientRect.bottom - clientRect.top);

            projectionMatrix = XMMatrixPerspectiveFovLH(XMConvertToRadians(fieldOfViewVertical), clientWidth / clientHeight, 0.1f, 1000000000.0f);

            d3dDeviceContext->UpdateSubresource(d3dConstantBuffers[CB_Application], 0, nullptr, &projectionMatrix, 0, 0);

            // Create the depth buffer for use with the depth/stencil view.
            D3D11_TEXTURE2D_DESC depthStencilBufferDesc;
            ZeroMemory(&depthStencilBufferDesc, sizeof(D3D11_TEXTURE2D_DESC));

            depthStencilBufferDesc.ArraySize = 1;
            depthStencilBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
            depthStencilBufferDesc.CPUAccessFlags = 0; // No CPU access required.
            depthStencilBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
            depthStencilBufferDesc.Width = width;
            depthStencilBufferDesc.Height = height;
            depthStencilBufferDesc.MipLevels = 1;
            depthStencilBufferDesc.SampleDesc.Count = 1;
            depthStencilBufferDesc.SampleDesc.Quality = 0;
            depthStencilBufferDesc.Usage = D3D11_USAGE_DEFAULT;

            HRESULT hr = d3dDevice->CreateTexture2D(&depthStencilBufferDesc, nullptr, &d3dDepthStencilBuffer);
            if(FAILED(hr))
            {
                LOGERROR("Failed to create depth stencil buffer!");
                return;
            }

            hr = d3dDevice->CreateDepthStencilView(d3dDepthStencilBuffer, nullptr, &d3dDepthStencilView);
            if(FAILED(hr))
            {
                LOGERROR("Failed to create depth stencil view!");
                return;
            }

            d3dDeviceContext->OMSetRenderTargets(1, &d3dRenderTargetView, d3dDepthStencilView);
        }
        else
        {
            windowMinimized = true;
        }
    }
}

void GraphicsSystem::Update(float dt)
{
    //Temp toggle mouse lock
    if(InputSystem::GetKeyPressed('L'))
    {
        lockMouseCenter = !lockMouseCenter;
        ShowCursor(!lockMouseCenter);
    }

    //Lock cursor position
    if(lockMouseCenter)
    {
        POINT center = {windowWidth / 2, windowHeight / 2 };
        ClientToScreen(windowHandle, &center);
        SetCursorPos(center.x, center.y);
    }

    if(!windowMinimized)Render(dt);
}

ID3D11PixelShader *GraphicsSystem::GetPixelShader(std::string name)
{
    auto it = pixelShaders.find(name);
    if(it != pixelShaders.end())
        return it->second;
    return LoadPixelShader(name, GS::to_wstring(name));
}
ID3D11VertexShader *GraphicsSystem::GetVertexShader(std::string name)
{
    auto it = vertexShaders.find(name);
    if(it != vertexShaders.end())
        return it->second;
    return LoadVertexShader(name, GS::to_wstring(name));
}

ID3D11PixelShader *GraphicsSystem::LoadPixelShader(std::string fileName, std::wstring fileNameWide)
{
    std::string path = fileName;
    std::string str = "Loading pixel shader: " + path;
    LOGDEBUG(str);

    // Load the compiled pixel shader.
    ID3DBlob* pixelShaderBlob;

    auto hr = D3DReadFileToBlob(fileNameWide.c_str(), &pixelShaderBlob);
    if(FAILED(hr))
    {
        std::string wrnstr = "Could not read " + path;
        LOGWARNING(wrnstr);
        return 0;
    }

    hr = d3dDevice->CreatePixelShader(pixelShaderBlob->GetBufferPointer(), pixelShaderBlob->GetBufferSize(), nullptr, &pixelShaders[fileName]);
    if(FAILED(hr))
    {
        std::string wrnstr = "Could not create pixel shader " + fileName;
        LOGWARNING(wrnstr);
        return 0;
    }

    SafeRelease(pixelShaderBlob);
    return pixelShaders[fileName];
}

ID3D11VertexShader *GraphicsSystem::LoadVertexShader(std::string fileName, std::wstring fileNameWide)
{
    std::string path = fileName;
    std::string str = "Loading vertex shader: " + path;
    LOGDEBUG(str);

    // Load the compiled pixel shader.
    ID3DBlob* vertexShaderBlob;

    auto hr = D3DReadFileToBlob(fileNameWide.c_str(), &vertexShaderBlob);
    if(FAILED(hr))
    {
        std::string wrnstr = "Could not read " + path;
        LOGWARNING(wrnstr);
        return 0;
    }

    hr = d3dDevice->CreateVertexShader(vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), nullptr, &vertexShaders[fileName]);
    if(FAILED(hr))
    {
        std::string wrnstr = "Could not create vertex shader " + fileName;
        LOGWARNING(wrnstr);
        return 0;
    }

    SafeRelease(vertexShaderBlob);
    return vertexShaders[fileName];
}

void GraphicsSystem::LoadInputLayouts()
{
    D3D11_INPUT_ELEMENT_DESC vertexLayoutDesc[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexPosCol, position), D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexPosCol, color), D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };

    ID3DBlob* vertexShaderBlob;
    auto hr = D3DReadFileToBlob(L"BasicVertexShader.cso", &vertexShaderBlob);

    hr = d3dDevice->CreateInputLayout(vertexLayoutDesc, _countof(vertexLayoutDesc), vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), &inputLayouts[InputLayout::POSCOL]);
    SafeRelease(vertexShaderBlob);
    if(FAILED(hr))
        LOGWARNING("Failed to create input layout POSCOL!");
    else
        LOGDEBUG("Created input layout POSCOL");

    D3D11_INPUT_ELEMENT_DESC vertexLayoutDescPosColAlpha[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexPosColAlpha, position), D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, offsetof(VertexPosColAlpha, colorAlpha), D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };

    hr = D3DReadFileToBlob(L"BasicParticleVS.cso", &vertexShaderBlob);

    hr = d3dDevice->CreateInputLayout(vertexLayoutDescPosColAlpha, _countof(vertexLayoutDescPosColAlpha), vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), &inputLayouts[InputLayout::POSCOLALPHA]);

    SafeRelease(vertexShaderBlob);
    if(FAILED(hr))
        LOGWARNING("Failed to create input layout POSCOLALPHA!");
    else
        LOGDEBUG("Created input layout POSCOLALPHA");

    D3D11_INPUT_ELEMENT_DESC vertexLayoutDesc1[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexPosUV, position), D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, offsetof(VertexPosUV, uv), D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };

    hr = D3DReadFileToBlob(L"UITextureVS.cso", &vertexShaderBlob);

    hr = d3dDevice->CreateInputLayout(vertexLayoutDesc1, _countof(vertexLayoutDesc1), vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), &inputLayouts[InputLayout::POSUV]);
    SafeRelease(vertexShaderBlob);
    if(FAILED(hr))
        LOGWARNING("Failed to create input layout POSUV!");
    else
        LOGDEBUG("Created input layout POSUV");

    D3D11_INPUT_ELEMENT_DESC vertexLayoutDesc2[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexPosUVNorm, position), D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, offsetof(VertexPosUVNorm, uv), D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexPosUVNorm, normal), D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };

    hr = D3DReadFileToBlob(L"BasicTextureVS.cso", &vertexShaderBlob);

    hr = d3dDevice->CreateInputLayout(vertexLayoutDesc2, _countof(vertexLayoutDesc2), vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), &inputLayouts[InputLayout::POSUVNORM]);
    SafeRelease(vertexShaderBlob);
    if(FAILED(hr))
        LOGWARNING("Failed to create input layout POSUVNORM!");
    else
        LOGDEBUG("Created input layout POSUVNORM");
}

void GraphicsSystem::CreateConstantBuffers()
{
    //Create per frame and per application constant buffers
    D3D11_BUFFER_DESC constantBufferDesc;
    ZeroMemory(&constantBufferDesc, sizeof(D3D11_BUFFER_DESC));

    constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    constantBufferDesc.ByteWidth = sizeof(XMMATRIX);
    constantBufferDesc.CPUAccessFlags = 0;
    constantBufferDesc.Usage = D3D11_USAGE_DEFAULT;

    D3D11_BUFFER_DESC constantBufferFrameDesc;
    ZeroMemory(&constantBufferFrameDesc, sizeof(D3D11_BUFFER_DESC));

    constantBufferFrameDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    constantBufferFrameDesc.ByteWidth = sizeof(FrameConstantBuffer);
    constantBufferFrameDesc.CPUAccessFlags = 0;
    constantBufferFrameDesc.Usage = D3D11_USAGE_DEFAULT;

    auto hr = d3dDevice->CreateBuffer(&constantBufferDesc, nullptr, &d3dConstantBuffers[CB_Application]);
    if(FAILED(hr))
        LOGERROR("Failed to create constant buffer!");
    hr = d3dDevice->CreateBuffer(&constantBufferFrameDesc, nullptr, &d3dConstantBuffers[CB_Frame]);
    if(FAILED(hr))
        LOGERROR("Failed to create constant buffer!");
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
//Init windows application
void GraphicsSystem::InitApplication(HINSTANCE hInstance, int cmdShow)
{
    WNDCLASSEX wndClass = { 0 };
    wndClass.cbSize = sizeof(WNDCLASSEX);
    wndClass.style = CS_HREDRAW | CS_VREDRAW; //Redraw window on resize (horizontal/vertical)
    wndClass.lpfnWndProc = &WndProc;
    wndClass.hInstance = hInstance;
    wndClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wndClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wndClass.lpszMenuName = nullptr;
    wndClass.lpszClassName = windowClassNameW;

    if(!RegisterClassEx(&wndClass))
    {
        MessageBox(nullptr, TEXT("Could not register window class!"), TEXT("Error"), MB_OK);
        return;
    }

    RECT windowRect = { 0, 0, windowWidth, windowHeight };
    AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

    windowHandle = CreateWindowA(windowClassName, windowName,
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
        windowRect.right - windowRect.left,
        windowRect.bottom - windowRect.top,
        nullptr, nullptr, hInstance, nullptr);

    if(!windowHandle)
    {
        MessageBox(nullptr, TEXT("Could not create window!"), TEXT("Error"), MB_OK);
        return;
    }

    ShowWindow(windowHandle, cmdShow);
    UpdateWindow(windowHandle);
}

void GraphicsSystem::InitDirectX(HINSTANCE hInstance)
{
    // A window handle must have been created already.
    assert(windowHandle != 0);

    RECT clientRect;
    GetClientRect(windowHandle, &clientRect);

    // Compute the exact client dimensions. This will be used
    // to initialize the render targets for our swap chain.
    unsigned int clientWidth = clientRect.right - clientRect.left;
    unsigned int clientHeight = clientRect.bottom - clientRect.top;

    DXGI_SWAP_CHAIN_DESC swapChainDesc;
    ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));

    swapChainDesc.BufferCount = 1;
    swapChainDesc.BufferDesc.Width = clientWidth;
    swapChainDesc.BufferDesc.Height = clientHeight;
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferDesc.RefreshRate = DXGI_RATIONAL{ 0, 1 };
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.OutputWindow = windowHandle;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    swapChainDesc.Windowed = TRUE;

    UINT createDeviceFlags = 0;
#if _DEBUG
    createDeviceFlags = D3D11_CREATE_DEVICE_DEBUG;
#endif

    // These are the feature levels that we will accept.
    D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0
    };

    // This will be the feature level that 
    // is used to create our device and swap chain.
    D3D_FEATURE_LEVEL featureLevel;

    HRESULT hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE,
        nullptr, createDeviceFlags, featureLevels, _countof(featureLevels),
        D3D11_SDK_VERSION, &swapChainDesc, &d3dSwapChain, &d3dDevice, &featureLevel,
        &d3dDeviceContext);

    //Used if feature level 11_1 is unavailable
    if(hr == E_INVALIDARG)
    {
        hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE,
            nullptr, createDeviceFlags, &featureLevels[1], _countof(featureLevels) - 1,
            D3D11_SDK_VERSION, &swapChainDesc, &d3dSwapChain, &d3dDevice, &featureLevel,
            &d3dDeviceContext);
    }

    if(FAILED(hr))
    {
        LOGERROR("Failed to create device and swap chain!");
        return;
    }

    // Next initialize the back buffer of the swap chain and associate it to a 
    // render target view.
    ID3D11Texture2D* backBuffer = 0;
    hr = d3dSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer);
    if(FAILED(hr))
    {
        LOGERROR("Failed to get back buffer!");
        return;
    }

    hr = d3dDevice->CreateRenderTargetView(backBuffer, nullptr, &d3dRenderTargetView);
    if(FAILED(hr))
    {
        LOGERROR("Failed to create render target view!");
        return;
    }

    SafeRelease(backBuffer);

    // Create the depth buffer for use with the depth/stencil view.
    D3D11_TEXTURE2D_DESC depthStencilBufferDesc;
    ZeroMemory(&depthStencilBufferDesc, sizeof(D3D11_TEXTURE2D_DESC));

    depthStencilBufferDesc.ArraySize = 1;
    depthStencilBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depthStencilBufferDesc.CPUAccessFlags = 0; // No CPU access required.
    depthStencilBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthStencilBufferDesc.Width = clientWidth;
    depthStencilBufferDesc.Height = clientHeight;
    depthStencilBufferDesc.MipLevels = 1;
    depthStencilBufferDesc.SampleDesc.Count = 1;
    depthStencilBufferDesc.SampleDesc.Quality = 0;
    depthStencilBufferDesc.Usage = D3D11_USAGE_DEFAULT;

    hr = d3dDevice->CreateTexture2D(&depthStencilBufferDesc, nullptr, &d3dDepthStencilBuffer);
    if(FAILED(hr))
    {
        LOGERROR("Failed to create depth stencil buffer!");
        return;
    }

    hr = d3dDevice->CreateDepthStencilView(d3dDepthStencilBuffer, nullptr, &d3dDepthStencilView);
    if(FAILED(hr))
    {
        LOGERROR("Failed to create depth stencil view!");
        return;
    }

    // Setup depth/stencil state.
    D3D11_DEPTH_STENCIL_DESC depthStencilStateDesc;
    ZeroMemory(&depthStencilStateDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));

    depthStencilStateDesc.DepthEnable = TRUE;
    depthStencilStateDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depthStencilStateDesc.DepthFunc = D3D11_COMPARISON_LESS;
    depthStencilStateDesc.StencilEnable = FALSE;

    hr = d3dDevice->CreateDepthStencilState(&depthStencilStateDesc, &d3dDepthStencilState);

    // Setup rasterizer state.
    D3D11_RASTERIZER_DESC rasterizerDesc;
    ZeroMemory(&rasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));

    rasterizerDesc.AntialiasedLineEnable = FALSE;
    rasterizerDesc.CullMode = D3D11_CULL_BACK;
    rasterizerDesc.DepthBias = 0;
    rasterizerDesc.DepthBiasClamp = 0.0f;
    rasterizerDesc.DepthClipEnable = TRUE;
    rasterizerDesc.FillMode = D3D11_FILL_SOLID;
    rasterizerDesc.FrontCounterClockwise = TRUE;
    rasterizerDesc.MultisampleEnable = FALSE;
    rasterizerDesc.ScissorEnable = FALSE;
    rasterizerDesc.SlopeScaledDepthBias = 0.0f;

    // Create the rasterizer state object.
    hr = d3dDevice->CreateRasterizerState(&rasterizerDesc, &d3dRasterizerState);
    if(FAILED(hr))
    {
        LOGERROR("Failed to create rasterizer state!");
        return;
    }

    // Initialize the viewport to occupy the entire client area.
    viewport.Width = static_cast<float>(clientWidth);
    viewport.Height = static_cast<float>(clientHeight);
    viewport.TopLeftX = 0.0f;
    viewport.TopLeftY = 0.0f;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;

    //Setup blend state
    D3D11_BLEND_DESC blendDesc;
    ZeroMemory(&blendDesc, sizeof(D3D11_BLEND_DESC));
    blendDesc.RenderTarget[0].BlendEnable = TRUE;
    blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
    blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

    hr = d3dDevice->CreateBlendState(&blendDesc, &d3dBlendState);
    if(FAILED(hr))
    {
        LOGERROR("Failed to create blend state!");
        return;
    }

    d3dDeviceContext->OMSetBlendState(d3dBlendState, 0, 0xffffffff);

    //Setup sampler state
    D3D11_SAMPLER_DESC samplerDesc;

    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.MinLOD = -FLT_MAX;
    samplerDesc.MaxLOD = FLT_MAX;
    samplerDesc.MipLODBias = 0.0f;
    samplerDesc.MaxAnisotropy = 1;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;


    hr = d3dDevice->CreateSamplerState(&samplerDesc, &d3dSamplerState);
    if(FAILED(hr))
    {
        LOGERROR("Failed to create sampler state!");
        return;
    }
    d3dDeviceContext->PSSetSamplers(0, 1, &d3dSamplerState);

}

// Clear the color and depth buffers.
void GraphicsSystem::Clear(const FLOAT clearColor[4], FLOAT clearDepth, UINT8 clearStencil)
{
    d3dDeviceContext->ClearRenderTargetView(d3dRenderTargetView, clearColor);
    d3dDeviceContext->ClearDepthStencilView(d3dDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, clearDepth, clearStencil);
}

void GraphicsSystem::Render(float dt)
{
    Clear(Colors::SkyBlue, 1.0f, 0);

    XMVECTOR upDirection = XMVectorSet(0, 1, 0, 0);

    FrameConstantBuffer frameBuf;
    frameBuf.viewMatrix = XMMatrixLookAtLH(eyePosition, focusPoint, upDirection);

    //Set Frame constant buffer
    //Temp hard code light values
    frameBuf.Ia = XMFLOAT4(0.2f, 0.2f, 0.2f, 0.0f);
    frameBuf.Ld = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.0f);
    frameBuf.Ls = XMFLOAT4(0.5f, 0.5f, 0.5f, 0.0f);
    CBY::Vector lightDir(0.2f, 0.8f, -0.4f);
    lightDir.Normalize();
    frameBuf.l = XMFLOAT4(lightDir.x, lightDir.y, lightDir.z, 0.0f);

    Transform *pTrans = PlayerController::GetPlayerTrans();
    frameBuf.eyePos = XMFLOAT4(pTrans->pos.x, pTrans->pos.y, pTrans->pos.z, 0.0f);

    d3dDeviceContext->UpdateSubresource(d3dConstantBuffers[CB_Frame], 0, nullptr, &frameBuf, 0, 0);
    
    //Use to render transparent objects last (object, distsq from camera)
    std::vector<std::pair<GameObject *, float>> transparentObjects;
    CBY::Vector camPos = PlayerController::GetPlayerTrans()->pos;
    //Call render object on each object with a render component
    for(auto &it : ObjectManagerSystem::gameObjects)
    {
        RenderComponent *renderComp = it->GetComponent<RenderComponent>();
        if(renderComp && renderComp->renderComponent)
        {
            if(renderComp->transparent)
            {
                Transform *pTrans = it->GetComponent<Transform>();
                float distSq = (pTrans->pos.x - camPos.x) * (pTrans->pos.x - camPos.x) + (pTrans->pos.y - camPos.y) * (pTrans->pos.y - camPos.y) + (pTrans->pos.z - camPos.z) * (pTrans->pos.z - camPos.z);
                transparentObjects.push_back(std::pair<GameObject *, float>(it, distSq));
            }
            else
            {
                RenderObject(it, dt);
            }
        }
    }

    //Sort transparent objects back to front
    auto sortLambda = [](std::pair<GameObject *, float> const &a, std::pair<GameObject*, float> const& b) -> bool
    {
        return a.second > b.second;
    };
    std::sort(transparentObjects.begin(), transparentObjects.end(), sortLambda);

    //Render transparent objects after regular ones
    for(auto& it : transparentObjects)
    {
        RenderObject(it.first, dt);
    }

    d3dSwapChain->Present(enableVSync, 0);
}

void GraphicsSystem::RenderObject(GameObject* pObject, float dt)
{
    Transform *pTrans = pObject->GetComponent<Transform>();

    static Chunk *curChunk = 0;
    Chunk *newChunk = TerrainManagerSystem::GetChunkFromWorldCoordinates((int)pTrans->pos.x, (int)pTrans->pos.y, (int)pTrans->pos.z);

    // Update chunk constant buffer if necessary (new chunk)
    if(newChunk != curChunk)
    {
        curChunk = newChunk;

        if(curChunk)
        {
            d3dDeviceContext->VSSetConstantBuffers(3, 1, &curChunk->constantBuffer);
            d3dDeviceContext->PSSetConstantBuffers(3, 1, &curChunk->constantBuffer);
        }
    }

    DirectX::XMMATRIX translationMatrix = XMMatrixTranslation(pTrans->pos.x, pTrans->pos.y, pTrans->pos.z);
    DirectX::XMMATRIX scaleMatrix = XMMatrixScaling(pTrans->scale.x, pTrans->scale.y, pTrans->scale.z);
    DirectX::XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw(pTrans->rot.x, pTrans->rot.y, pTrans->rot.z);

    DirectX::XMMATRIX worldMatrix = rotationMatrix * scaleMatrix * translationMatrix;

    for(Mesh *pMesh : pObject->GetComponent<RenderComponent>()->pModel->meshes)
    {
        const UINT vertexStride = pMesh->material->GetVertexSize();
        const UINT offset = 0;

        //Render object
        pMesh->material->BindConstantBuffer(&worldMatrix, &rotationMatrix);
        pMesh->material->BindMaterial();
        d3dDeviceContext->IASetVertexBuffers(0, 1, &pMesh->vertexBuffer, &vertexStride, &offset);
        d3dDeviceContext->IASetIndexBuffer(pMesh->indexBuffer, DXGI_FORMAT_R32_UINT, 0);
        d3dDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        d3dDeviceContext->VSSetConstantBuffers(0, ConstantBuffer::NumConstantBuffers, d3dConstantBuffers);
        d3dDeviceContext->VSSetConstantBuffers(2, 1, &pMesh->material->constantBuffer);
        d3dDeviceContext->PSSetConstantBuffers(0, ConstantBuffer::NumConstantBuffers, d3dConstantBuffers);
        d3dDeviceContext->PSSetConstantBuffers(2, 1, &pMesh->material->constantBuffer);

        d3dDeviceContext->RSSetState(d3dRasterizerState);
        d3dDeviceContext->RSSetViewports(1, &viewport);

        d3dDeviceContext->OMSetRenderTargets(1, &d3dRenderTargetView, d3dDepthStencilView);
        d3dDeviceContext->OMSetDepthStencilState(d3dDepthStencilState, 1);

        d3dDeviceContext->DrawIndexed(pMesh->indexCount, 0, 0);
    }
}

void GraphicsSystem::SetCameraTrans(Transform *trans)
{
    eyePosition = XMVectorSet(trans->pos.x, trans->pos.y, trans->pos.z, 1.0f);
    //Calculate vector from euler rotation
    CBY::Vector rotVector(cos(trans->rot.y) * cos(trans->rot.x), sin(trans->rot.x), sin(trans->rot.y) * cos(trans->rot.x));
    focusPoint = XMVectorSet(rotVector.x + trans->pos.x, rotVector.y + trans->pos.y, rotVector.z + trans->pos.z, 1.0f);
}

