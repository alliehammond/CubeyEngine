//*********************************
//Precompiled header
//*********************************

//Local includes
#include "Core\LoggingSystem.h"
#include "Core\InputSystem.h"
#include "CYMath\Vector.h"
#include "CYMath\Vector4.h"
#include "Core\GameObject.h"
#include "Core\ObjectManagerSystem.h"
#include "Graphics\GraphicsSystem.h"
#include "Terrain\TerrainManagerSystem.h"
#include "Terrain\BlockType.h"
#include "Graphics\RenderComponent.h"
#include "Gameplay\PlayerController.h"

// System includes
#include <windows.h>

// DirectX includes
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <DirectXColors.h>

// STL includes
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <ctime>
#include <cstdlib>
#include <tuple>

// Link library dependencies
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "winmm.lib")

// Safely release a COM object.
template<typename T>
inline void SafeRelease(T& ptr)
{
    if (ptr != NULL)
    {
        ptr->Release();
        ptr = NULL;
    }
}

//Defines
#define PI 3.14159265358979f
#include "GameDefines.h"
