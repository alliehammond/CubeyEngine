#include "EnginePCH.h"
#include "windowsx.h"
#include "Core\CubeySystem.h"
#include "Graphics\RenderComponent.h"
#include "Gameplay\PlayerController.h"
#include "Terrain\Chunk.h"
#include "Terrain\TerrainManagerSystem.h"

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE prevInstance, _In_ LPWSTR cmdLine, _In_ int cmdShow)
{
    UNREFERENCED_PARAMETER(prevInstance);
    UNREFERENCED_PARAMETER(cmdLine);

    MSG msg = { 0 };

    static DWORD previousTime = timeGetTime();
    std::srand(unsigned int(std::time(nullptr)));

    //Initialize systems
    std::unique_ptr<CubeySystem> engineSystems[CubeySystems::SYSTEMCOUNT];
    engineSystems[CubeySystems::INPUTSYSTEM] = std::make_unique<InputSystem>();
    engineSystems[CubeySystems::LOGGINGSYSTEM] = std::make_unique<LoggingSystem>();
    engineSystems[CubeySystems::OBJECTMANAGERSYSTEM] = std::make_unique<ObjectManagerSystem>();
    engineSystems[CubeySystems::GRAPHICSSYSTEM] = std::make_unique<GraphicsSystem>(hInstance, cmdShow);
    engineSystems[CubeySystems::TERRAINMANAGERSYSTEM] = std::make_unique<TerrainManagerSystem>();

    //Create objects
    GameObject *player = CreateGameObject("Player");
    player->AddComponent<PlayerController>(std::make_unique<PlayerController>(player));
    player->GetComponent<Transform>()->pos.x = 1.0f;
    player->GetComponent<Transform>()->pos.y = 10.0f;   


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
        //Skip object manager deletion for last
        if(i != CubeySystems::OBJECTMANAGERSYSTEM)
            engineSystems[i].reset();
    }
    engineSystems[CubeySystems::OBJECTMANAGERSYSTEM].reset();

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
        }
        break;
        case WM_DESTROY:
        {
            PostQuitMessage(0);
        }
        break;
        case WM_SIZE:
        {
            GraphicsSystem::ResizeWindow(LOWORD(lParam), HIWORD(lParam));
        }
        break;
        case WM_KEYDOWN:
        {
            InputSystem::HandleWindowsMessageKeyDown(unsigned int(wParam));
        }
        break;
        case WM_KEYUP:
        {
            InputSystem::HandleWindowsMessageKeyUp(unsigned int(wParam));
        }
        break;
        case WM_MOUSEMOVE:
        {
            InputSystem::HandleMousePositionMessage(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        }
        break;
        default:
        return DefWindowProc(hwnd, message, wParam, lParam);
        }

    return 0;
}
