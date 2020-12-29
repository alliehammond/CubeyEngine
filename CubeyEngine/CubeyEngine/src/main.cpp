#include "EnginePCH.h"
#include "Core\CubeySystem.h"

//Main loop
int Run();

//Load resources to gpu (temp probs)
bool LoadContent();
void UnloadContent();

void Update(float deltaTime);
void Render();
void Cleanup();


int InitDirectX(HINSTANCE hInstance, BOOL vSync)
{
    // A window handle must have been created already.
    assert(_windowHandle != 0);

    RECT clientRect;
    GetClientRect(_windowHandle, &clientRect);

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
    swapChainDesc.BufferDesc.RefreshRate = DXGI_RATIONAL{0, 1};
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.OutputWindow = _windowHandle;
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
        D3D11_SDK_VERSION, &swapChainDesc, &_d3dSwapChain, &_d3dDevice, &featureLevel,
        &_d3dDeviceContext);

    //Used if feature level 11_1 is unavailable
    if (hr == E_INVALIDARG)
    {
        hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE,
            nullptr, createDeviceFlags, &featureLevels[1], _countof(featureLevels) - 1,
            D3D11_SDK_VERSION, &swapChainDesc, &_d3dSwapChain, &_d3dDevice, &featureLevel,
            &_d3dDeviceContext);
    }

    if (FAILED(hr))
    {
        return -1;
    }

    // Next initialize the back buffer of the swap chain and associate it to a 
    // render target view.
    ID3D11Texture2D* backBuffer;
    hr = _d3dSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer);
    if (FAILED(hr))
    {
        return -1;
    }

    hr = _d3dDevice->CreateRenderTargetView(backBuffer, nullptr, &_d3dRenderTargetView);
    if (FAILED(hr))
    {
        return -1;
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

    hr = _d3dDevice->CreateTexture2D(&depthStencilBufferDesc, nullptr, &_d3dDepthStencilBuffer);
    if (FAILED(hr))
    {
        return -1;
    }

    hr = _d3dDevice->CreateDepthStencilView(_d3dDepthStencilBuffer, nullptr, &_d3dDepthStencilView);
    if (FAILED(hr))
    {
        return -1;
    }

    // Setup depth/stencil state.
    D3D11_DEPTH_STENCIL_DESC depthStencilStateDesc;
    ZeroMemory(&depthStencilStateDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));

    depthStencilStateDesc.DepthEnable = TRUE;
    depthStencilStateDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depthStencilStateDesc.DepthFunc = D3D11_COMPARISON_LESS;
    depthStencilStateDesc.StencilEnable = FALSE;

    hr = _d3dDevice->CreateDepthStencilState(&depthStencilStateDesc, &_d3dDepthStencilState);

    // Setup rasterizer state.
    D3D11_RASTERIZER_DESC rasterizerDesc;
    ZeroMemory(&rasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));

    rasterizerDesc.AntialiasedLineEnable = FALSE;
    rasterizerDesc.CullMode = D3D11_CULL_BACK;
    rasterizerDesc.DepthBias = 0;
    rasterizerDesc.DepthBiasClamp = 0.0f;
    rasterizerDesc.DepthClipEnable = TRUE;
    rasterizerDesc.FillMode = D3D11_FILL_SOLID;
    rasterizerDesc.FrontCounterClockwise = FALSE;
    rasterizerDesc.MultisampleEnable = FALSE;
    rasterizerDesc.ScissorEnable = FALSE;
    rasterizerDesc.SlopeScaledDepthBias = 0.0f;

    // Create the rasterizer state object.
    hr = _d3dDevice->CreateRasterizerState(&rasterizerDesc, &_d3dRasterizerState);
    if (FAILED(hr))
    {
        return -1;
    }

    // Initialize the viewport to occupy the entire client area.
    _viewport.Width = static_cast<float>(clientWidth);
    _viewport.Height = static_cast<float>(clientHeight);
    _viewport.TopLeftX = 0.0f;
    _viewport.TopLeftY = 0.0f;
    _viewport.MinDepth = 0.0f;
    _viewport.MaxDepth = 1.0f;

    return 0;
}

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE prevInstance, _In_ LPWSTR cmdLine, _In_ int cmdShow)
{
    UNREFERENCED_PARAMETER(prevInstance);
    UNREFERENCED_PARAMETER(cmdLine);

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

void Update(float deltaTime)
{
    XMVECTOR eyePosition = XMVectorSet(0, 0, -10, 1);
    XMVECTOR focusPoint = XMVectorSet(0, 0, 0, 1);
    XMVECTOR upDirection = XMVectorSet(0, 1, 0, 0);
    _viewMatrix = XMMatrixLookAtLH(eyePosition, focusPoint, upDirection);
    _d3dDeviceContext->UpdateSubresource(_d3dConstantBuffers[CB_Frame], 0, nullptr, &_viewMatrix, 0, 0);


    static float angle = 0.0f;
    angle += 90.0f * deltaTime;
    XMVECTOR rotationAxis = XMVectorSet(0, 1, 1, 0);

    _worldMatrix = XMMatrixRotationAxis(rotationAxis, XMConvertToRadians(angle));
    _d3dDeviceContext->UpdateSubresource(_d3dConstantBuffers[CB_Object], 0, nullptr, &_worldMatrix, 0, 0);
}

// Clear the color and depth buffers.
void Clear(const FLOAT clearColor[4], FLOAT clearDepth, UINT8 clearStencil)
{
    _d3dDeviceContext->ClearRenderTargetView(_d3dRenderTargetView, clearColor);
    _d3dDeviceContext->ClearDepthStencilView(_d3dDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, clearDepth, clearStencil);
}

void Present(bool vSync)
{
    if (vSync)
    {
        _d3dSwapChain->Present(1, 0);
    }
    else
    {
        _d3dSwapChain->Present(0, 0);
    }
}

void Render()
{
    assert(_d3dDevice);
    assert(_d3dDeviceContext);

    Clear(Colors::DeepPink, 1.0f, 0);

    const UINT vertexStride = sizeof(VertexPosColor);
    const UINT offset = 0;

    _d3dDeviceContext->IASetVertexBuffers(0, 1, &_d3dVertexBuffer, &vertexStride, &offset);
    _d3dDeviceContext->IASetInputLayout(_d3dInputLayout);
    _d3dDeviceContext->IASetIndexBuffer(_d3dIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
    _d3dDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    _d3dDeviceContext->VSSetShader(_d3dVertexShader, nullptr, 0);
    _d3dDeviceContext->VSSetConstantBuffers(0, 3, _d3dConstantBuffers);
    _d3dDeviceContext->PSSetShader(_d3dPixelShader, nullptr, 0);

    _d3dDeviceContext->RSSetState(_d3dRasterizerState);
    _d3dDeviceContext->RSSetViewports(1, &_viewport);

    _d3dDeviceContext->OMSetRenderTargets(1, &_d3dRenderTargetView, _d3dDepthStencilView);
    _d3dDeviceContext->OMSetDepthStencilState(_d3dDepthStencilState, 1);

    _d3dDeviceContext->DrawIndexed(_countof(_indices), 0, 0);

    Present(_enableVSync);
}


//Main application loop
int Run()
{
    MSG msg = { 0 };

    static DWORD previousTime = timeGetTime();

    //Initialize systems
    CubeySystem* engineSystems[CubeySystems::SYSTEMCOUNT];
    engineSystems[CubeySystems::LOGGINGSYSTEM] = new LoggingSystem();
    engineSystems[CubeySystems::OBJECTMANAGERSYSTEM] = new ObjectManagerSystem();
    engineSystems[CubeySystems::GRAPHICSSYSTEM] = new GraphicsSystem();

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
            float targetDT = 1.0f/frameRateCap;
            if(deltaTime < targetDT)
            {
                //Cap framerate
                Sleep(int((targetDT - deltaTime) * 1000.0f));
            }

            //Update systems
            for (int i = 0; i < CubeySystems::SYSTEMCOUNT; ++i)
            {
                engineSystems[i]->Update(deltaTime);
            }

            Update(deltaTime);
            Render();
        }
    }

    //Cleanup systems
    for (int i = 0; i < CubeySystems::SYSTEMCOUNT; ++i)
    {
        delete engineSystems[i];
    }

    return static_cast<int>(msg.wParam);
}
