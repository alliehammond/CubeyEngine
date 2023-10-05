#include "EnginePCH.h"
#include "windowsx.h"
#include "Core\CubeySystem.h"
#include "Graphics\RenderComponent.h"
#include "Gameplay\PlayerController.h"
#include "Terrain\Chunk.h"
#include "Terrain\TerrainManagerSystem.h"
#include "Gameplay\UIComponent.h"

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE prevInstance, _In_ LPWSTR cmdLine, _In_ int cmdShow)
{
    UNREFERENCED_PARAMETER(prevInstance);
    UNREFERENCED_PARAMETER(cmdLine);

    MSG msg = { 0 };

    static DWORD previousTime = timeGetTime();
    std::srand(unsigned int(std::time(nullptr)));

    //Initialize systems
    CubeySystem* engineSystems[CubeySystems::SYSTEMCOUNT];
    engineSystems[CubeySystems::INPUTSYSTEM] = new InputSystem();
    engineSystems[CubeySystems::LOGGINGSYSTEM] = new LoggingSystem();
    engineSystems[CubeySystems::OBJECTMANAGERSYSTEM] = new ObjectManagerSystem();
    engineSystems[CubeySystems::GRAPHICSSYSTEM] = new GraphicsSystem(hInstance, cmdShow);
    engineSystems[CubeySystems::TERRAINMANAGERSYSTEM] = new TerrainManagerSystem();

    //Create objects
    GameObject *player = ObjectManagerSystem::CreateObject(new GameObject("Player"));
    player->AddComponent<PlayerController>(new PlayerController(player));
    player->GetComponent<Transform>()->pos.x = 1.0f;
    player->GetComponent<Transform>()->pos.y = 100.0f;

    GameObject *UI = ObjectManagerSystem::CreateObject(new GameObject("UI"));
    player->AddComponent<UIComponent>(new UIComponent(UI));

    //Test
    GameObject *test = ObjectManagerSystem::CreateObject(new GameObject("test"));
    test->AddComponent<RenderComponent>(new RenderComponent("BasicCube.fbx", new Material("BasicParticleVS.cso", "BasicPixelShader.cso", InputLayout::POSCOLALPHA, "testMat"), test, CBY::Vector4(1.0f, 0.0f, 0.0f, 0.4f)));
    test->GetComponent<Transform>()->pos.x = 1.0f;
    test->GetComponent<Transform>()->pos.y = 100.0f;


    while(msg.message != WM_QUIT)
    {
        if(PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            DWORD currentTime = timeGetTime();
            float deltaTime = (currentTime - previousTime) / 1000.0f;
            previousTime = currentTime;

            //Update systems
            for(int i = 0; i < CubeySystems::SYSTEMCOUNT; ++i)
            {
                engineSystems[i]->Update(deltaTime);
            }
        }
    }

    //Cleanup systems
    for(int i = 0; i < CubeySystems::SYSTEMCOUNT; ++i)
    {
        //Skip object manager and graphics system deletion for last
        if(i != CubeySystems::OBJECTMANAGERSYSTEM && i != CubeySystems::GRAPHICSSYSTEM)
            delete engineSystems[i];
    }
    delete engineSystems[CubeySystems::OBJECTMANAGERSYSTEM];
    delete engineSystems[CubeySystems::GRAPHICSSYSTEM];

    return 0;
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
            break;
        }
        case WM_DESTROY:
        {
            PostQuitMessage(0);
            break;
        }
        case WM_SIZE:
        {
            GraphicsSystem::ResizeWindow(LOWORD(lParam), HIWORD(lParam));
            break;
        }
        case WM_KEYDOWN:
        {
            InputSystem::HandleWindowsMessageKeyDown(unsigned int(wParam));
            break;
        }
        case WM_KEYUP:
        {
            InputSystem::HandleWindowsMessageKeyUp(unsigned int(wParam));
            break;
        }
        case WM_MOUSEMOVE:
        {
            InputSystem::HandleMousePositionMessage(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
            break;
        }
        case WM_LBUTTONDOWN:
        {
            InputSystem::HandleWindowsMessageLMB(true);
            break;
        }
        case WM_LBUTTONUP:
        {
            InputSystem::HandleWindowsMessageLMB(false);
            break;
        }
        case WM_RBUTTONDOWN:
        {
            InputSystem::HandleWindowsMessageRMB(true);
            break;
        }
        case WM_RBUTTONUP:
        {
            InputSystem::HandleWindowsMessageRMB(false);
            break;
        }
        case WM_MOUSEWHEEL:
        {
            InputSystem::HandleMouseScrollMessage(GET_WHEEL_DELTA_WPARAM(wParam));
            break;
        }
        default:
        return DefWindowProc(hwnd, message, wParam, lParam);
        }

    return 0;
}
