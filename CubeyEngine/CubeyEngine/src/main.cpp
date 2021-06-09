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
            float targetDT = 1.0f / CBYDefines::frameRateCap;
            if(deltaTime < targetDT)
            {
                //Cap framerate
                Sleep(int((targetDT - deltaTime) * 1000.0f));
            }

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
            delete engineSystems[i];
    }
    delete engineSystems[CubeySystems::OBJECTMANAGERSYSTEM];

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
