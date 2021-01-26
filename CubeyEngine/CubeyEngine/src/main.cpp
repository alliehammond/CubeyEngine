#include "EnginePCH.h"
#include "Core\CubeySystem.h"
#include "Graphics\RenderComponent.h"

const float frameRateCap = 120.0f;

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE prevInstance, _In_ LPWSTR cmdLine, _In_ int cmdShow)
{
    UNREFERENCED_PARAMETER(prevInstance);
    UNREFERENCED_PARAMETER(cmdLine);

    MSG msg = { 0 };

    static DWORD previousTime = timeGetTime();
    std::srand(unsigned int(std::time(nullptr)));

    //Initialize systems
    CubeySystem* engineSystems[CubeySystems::SYSTEMCOUNT];
    engineSystems[CubeySystems::LOGGINGSYSTEM] = new LoggingSystem();
    engineSystems[CubeySystems::OBJECTMANAGERSYSTEM] = new ObjectManagerSystem();
    engineSystems[CubeySystems::GRAPHICSSYSTEM] = new GraphicsSystem(hInstance, cmdShow);

    //Load temp cube
    GameObject *tempCube = ObjectManagerSystem::CreateObject(new GameObject("Cube!"));
    RenderComponent *tempRComp = tempCube->AddComponent<RenderComponent>(
        new RenderComponent("BasicCube.obj", tempCube));

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
            float targetDT = 1.0f / frameRateCap;
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
        delete engineSystems[i];
    }

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
        default:
        return DefWindowProc(hwnd, message, wParam, lParam);
        }

    return 0;
}
