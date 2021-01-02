#include "EnginePCH.h"
#include "Graphics\GraphicsSystem.h"

using namespace DirectX;

GraphicsSystem::GraphicsSystem(HINSTANCE hInstance, int cmdShow)
{
    LOGDEBUG("Graphics system initializing...");

    InitApplication(hInstance, cmdShow);
    InitDirectX(hInstance);

    CreateConstantBuffers();

    LOGDEBUG("Loading vertex shaders...");
    LoadVertexShader("BasicVertexShader.cso", L"BasicVertexShader.cso");

    LOGDEBUG("Loading pixel shaders...");
    LoadPixelShader("BasicPixelShader.cso", L"BasicPixelShader.cso");

    LOGDEBUG("Loading input layouts...");
    LoadInputLayouts();

    // Setup the projection matrix
    RECT clientRect;
    GetClientRect(windowHandle, &clientRect);

    // Compute the exact client dimensions
    // This is required for a correct projection matrix
    float clientWidth = static_cast<float>(clientRect.right - clientRect.left);
    float clientHeight = static_cast<float>(clientRect.bottom - clientRect.top);

    projectionMatrix = XMMatrixPerspectiveFovLH(XMConvertToRadians(45.0f), clientWidth / clientHeight, 0.1f, 100.0f);

    d3dDeviceContext->UpdateSubresource(d3dConstantBuffers[CB_Application], 0, nullptr, &projectionMatrix, 0, 0);


    LOGDEBUG("Graphics system initialized!");
}

GraphicsSystem::~GraphicsSystem()
{
    SafeRelease(d3dConstantBuffers[CB_Application]);
    SafeRelease(d3dConstantBuffers[CB_Frame]);
    SafeRelease(d3dConstantBuffers[CB_Object]);
    SafeRelease(d3dIndexBuffer);
    SafeRelease(d3dVertexBuffer);

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
    vertexShaders.clear();
    pixelShaders.clear();
    inputLayouts.clear();
    

    SafeRelease(d3dDepthStencilView);
    SafeRelease(d3dRenderTargetView);
    SafeRelease(d3dDepthStencilBuffer);
    SafeRelease(d3dDepthStencilState);
    SafeRelease(d3dRasterizerState);
    SafeRelease(d3dSwapChain);
    SafeRelease(d3dDeviceContext);
    SafeRelease(d3dDevice);
}

void GraphicsSystem::Update(float dt)
{
    //TEMP
    XMVECTOR eyePosition = XMVectorSet(0, 0, -10, 1);
    XMVECTOR focusPoint = XMVectorSet(0, 0, 0, 1);
    XMVECTOR upDirection = XMVectorSet(0, 1, 0, 0);
    viewMatrix = XMMatrixLookAtLH(eyePosition, focusPoint, upDirection);
    d3dDeviceContext->UpdateSubresource(d3dConstantBuffers[CB_Frame], 0, nullptr, &viewMatrix, 0, 0);


    static float angle = 0.0f;
    angle += 90.0f * dt;
    XMVECTOR rotationAxis = XMVectorSet(0, 1, 1, 0);

    worldMatrix = XMMatrixRotationAxis(rotationAxis, XMConvertToRadians(angle));
    d3dDeviceContext->UpdateSubresource(d3dConstantBuffers[CB_Object], 0, nullptr, &worldMatrix, 0, 0);
    //ENDTEMP

    Render();
}

void GraphicsSystem::LoadPixelShader(std::string fileName, std::wstring fileNameWide)
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
    }

    hr = d3dDevice->CreatePixelShader(pixelShaderBlob->GetBufferPointer(), pixelShaderBlob->GetBufferSize(), nullptr, &pixelShaders[fileName]);
    if(FAILED(hr))
    {
        std::string wrnstr = "Could not create pixel shader " + fileName;
        LOGWARNING(wrnstr);
    }

    SafeRelease(pixelShaderBlob);
}

void GraphicsSystem::LoadVertexShader(std::string fileName, std::wstring fileNameWide)
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
    }

    hr = d3dDevice->CreateVertexShader(vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), nullptr, &vertexShaders[fileName]);
    if(FAILED(hr))
    {
        std::string wrnstr = "Could not create vertex shader " + fileName;
        LOGWARNING(wrnstr);
    }

    SafeRelease(vertexShaderBlob);
}

void GraphicsSystem::LoadInputLayouts()
{
    D3D11_INPUT_ELEMENT_DESC vertexLayoutDesc[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexPosColor, position), D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexPosColor, color), D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };

    ID3DBlob* vertexShaderBlob;
    auto hr = D3DReadFileToBlob(L"BasicVertexShader.cso", &vertexShaderBlob);

    hr = d3dDevice->CreateInputLayout(vertexLayoutDesc, _countof(vertexLayoutDesc), vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), &inputLayouts[InputLayout::POSCOL]);
    SafeRelease(vertexShaderBlob);
    if(FAILED(hr))
        LOGWARNING("Failed to create input layout POSCOL!");
    else
        LOGDEBUG("Created input layout POSCOL");
}

void GraphicsSystem::CreateConstantBuffers()
{
    // Create the constant buffers for the variables defined in the vertex shader.
    D3D11_BUFFER_DESC constantBufferDesc;
    ZeroMemory(&constantBufferDesc, sizeof(D3D11_BUFFER_DESC));

    constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    constantBufferDesc.ByteWidth = sizeof(XMMATRIX);
    constantBufferDesc.CPUAccessFlags = 0;
    constantBufferDesc.Usage = D3D11_USAGE_DEFAULT;

    auto hr = d3dDevice->CreateBuffer(&constantBufferDesc, nullptr, &d3dConstantBuffers[CB_Application]);
    if(FAILED(hr))
        LOGERROR("Failed to create constant buffer!");
    hr = d3dDevice->CreateBuffer(&constantBufferDesc, nullptr, &d3dConstantBuffers[CB_Frame]);
    if(FAILED(hr))
        LOGERROR("Failed to create constant buffer!");
    hr = d3dDevice->CreateBuffer(&constantBufferDesc, nullptr, &d3dConstantBuffers[CB_Object]);
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
    ID3D11Texture2D* backBuffer;
    hr = d3dSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer);
    if(FAILED(hr))
    {
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
        return;
    }

    hr = d3dDevice->CreateDepthStencilView(d3dDepthStencilBuffer, nullptr, &d3dDepthStencilView);
    if(FAILED(hr))
    {
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
        return;
    }

    // Initialize the viewport to occupy the entire client area.
    viewport.Width = static_cast<float>(clientWidth);
    viewport.Height = static_cast<float>(clientHeight);
    viewport.TopLeftX = 0.0f;
    viewport.TopLeftY = 0.0f;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
}

// Clear the color and depth buffers.
void GraphicsSystem::Clear(const FLOAT clearColor[4], FLOAT clearDepth, UINT8 clearStencil)
{
    d3dDeviceContext->ClearRenderTargetView(d3dRenderTargetView, clearColor);
    d3dDeviceContext->ClearDepthStencilView(d3dDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, clearDepth, clearStencil);
}

void GraphicsSystem::Render()
{
    Clear(Colors::DeepPink, 1.0f, 0);

    const UINT vertexStride = sizeof(VertexPosColor);
    const UINT offset = 0;

    d3dDeviceContext->IASetVertexBuffers(0, 1, &d3dVertexBuffer, &vertexStride, &offset);
    d3dDeviceContext->IASetInputLayout(inputLayouts[InputLayout::POSCOL]);
    d3dDeviceContext->IASetIndexBuffer(d3dIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
    d3dDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    d3dDeviceContext->VSSetShader(vertexShaders["BasicVertexShader.cso"], nullptr, 0);
    d3dDeviceContext->VSSetConstantBuffers(0, 3, d3dConstantBuffers);
    d3dDeviceContext->PSSetShader(pixelShaders["BasicPixelShader.cso"], nullptr, 0);

    d3dDeviceContext->RSSetState(d3dRasterizerState);
    d3dDeviceContext->RSSetViewports(1, &viewport);

    d3dDeviceContext->OMSetRenderTargets(1, &d3dRenderTargetView, d3dDepthStencilView);
    d3dDeviceContext->OMSetDepthStencilState(d3dDepthStencilState, 1);

    d3dDeviceContext->DrawIndexed(_countof(_indices), 0, 0);

    d3dSwapChain->Present(enableVSync, 0);
}
