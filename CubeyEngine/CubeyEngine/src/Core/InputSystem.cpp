#include "EnginePCH.h"
#include "Core\InputSystem.h"
#include <vector>

std::vector<bool> InputSystem::KeysDown((unsigned)KeyCode::NUMCODES);
std::vector<bool> InputSystem::KeysDownPrev((unsigned)KeyCode::NUMCODES);
std::vector<bool> InputSystem::KeysPressed((unsigned)KeyCode::NUMCODES);
std::vector<bool> InputSystem::KeysReleased((unsigned)KeyCode::NUMCODES);

unsigned InputSystem::mouseX = 0, InputSystem::mouseY = 0;
int InputSystem::mouseDeltaX = 0, InputSystem::mouseDeltaY = 0;
int InputSystem::mouseWheelDelta = 0, InputSystem::mouseWheelDeltaPrevFrame = 0;

//Vector character layout:
//0-25 = 'A'-'Z'
//26-35 = '0'-'9'
//36-40 = CTRL, SHIFT, ALT, LEFTMOUSE, RIGHTMOUSE

InputSystem::InputSystem()
{
    for(int i = 0;i < KeysDown.size(); ++i)
    {
        KeysDown[i] = false;
        KeysPressed[i] = false;
        KeysReleased[i] = false;
        KeysDownPrev[i] = false;
    }
}

InputSystem::~InputSystem() { }

void InputSystem::Update(float dt)
{
    //Update Keys pressed and released
    for(int i = 0;i < KeysDown.size(); ++i)
    {
        if(KeysDown[i] && !KeysDownPrev[i])
            KeysPressed[i] = true;
        else
            KeysPressed[i] = false;

        if(!KeysDown[i] && KeysDownPrev[i])
            KeysReleased[i] = true;
        else
            KeysReleased[i] = false;
    }

    for(int i = 0;i < KeysDown.size(); ++i)
    {
        KeysDownPrev[i] = KeysDown[i];
    }

    //Update mouse delta x/y
    mouseDeltaX = 0;
    mouseDeltaY = 0;
    mouseWheelDelta = mouseWheelDeltaPrevFrame;
    mouseWheelDeltaPrevFrame = 0;
    //Only update mouse deltas if cursor is locked to center of screen
    static bool ignoreFirstLockFrame = true;//If true ignore first frame where mouse moves to center
    if(GraphicsSystem::GetMouseCursorLock())
    {
        if(!ignoreFirstLockFrame)
        {
            mouseDeltaX = (int)mouseX - GraphicsSystem::GetWindowWidth() / 2;
            mouseDeltaY = -((int)mouseY - GraphicsSystem::GetWindowHeight() / 2);
        }
        ignoreFirstLockFrame = false;
    }
    else
    {
        ignoreFirstLockFrame = true;
    }
}

bool InputSystem::GetKeyDown(char key)
{
    if(key >= 'A' && key <= 'Z')
    {
        return KeysDown[0 + key - 'A'];
    }
    if(key >= '0' && key <= '9')
    {
        return KeysDown[26 + key - '0'];
    }

    LOGWARNING("InputSystem: Invalid key code checked! " + std::to_string(key));
    return false;
}

bool InputSystem::GetKeyPressed(char key)
{
    if(key >= 'A' && key <= 'Z')
    {
        return KeysPressed[0 + key - 'A'];
    }
    if(key >= '0' && key <= '9')
    {
        return KeysPressed[26 + key - '0'];
    }
    LOGWARNING("InputSystem: Invalid key code checked! " + std::to_string(key));
    return false;
}

bool InputSystem::GetKeyReleased(char key)
{
    if(key >= 'A' && key <= 'Z')
    {
        return KeysReleased[0 + key - 'A'];
    }
    if(key >= '0' && key <= '9')
    {
        return KeysReleased[26 + key - '0'];
    }
    LOGWARNING("InputSystem: Invalid key code checked! " + std::to_string(key));
    return false;
}

bool InputSystem::GetKeyDown(KeyCode key)
{
    if(key >= KeyCode::CTRL && key < KeyCode::NUMCODES)
    {
        return KeysDown[(unsigned char)key];
    }
    LOGWARNING("InputSystem: Invalid key code modifier checked!");
    return false;
}

bool InputSystem::GetKeyPressed(KeyCode key)
{
    if(key >= KeyCode::CTRL && key < KeyCode::NUMCODES)
    {
        return KeysPressed[(unsigned char)key];
    }
    LOGWARNING("InputSystem: Invalid key code modifier checked!");
    return false;
}

bool InputSystem::GetKeyReleased(KeyCode key)
{
    if(key >= KeyCode::CTRL && key < KeyCode::NUMCODES)
    {
        return KeysReleased[(unsigned char)key];
    }
    LOGWARNING("InputSystem: Invalid key code modifier checked!");
    return false;
}

void InputSystem::HandleWindowsMessageKeyDown(unsigned int code)
{
    if(code >= 'A' && code <= 'Z')
    {
        KeysDown[0 + code - 'A'] = true;
    }
    if(code >= '0' && code <= '9')
    {
        KeysDown[26 + code - '0'] = true;
    }
    switch(code)
    {
    case VK_CONTROL:
        KeysDown[(unsigned)KeyCode::CTRL] = true;
        break;
    case VK_SHIFT:
        KeysDown[(unsigned)KeyCode::SHIFT] = true;
        break;
    case VK_MENU:
        KeysDown[(unsigned)KeyCode::ALT] = true;
        break;
    }
}

void InputSystem::HandleWindowsMessageKeyUp(unsigned int code)
{
    if(code >= 'A' && code <= 'Z')
    {
        KeysDown[0 + code - 'A'] = false;
    }
    if(code >= '0' && code <= '9')
    {
        KeysDown[26 + code - '0'] = false;
    }
    switch(code)
    {
    case VK_CONTROL:
        KeysDown[(unsigned)KeyCode::CTRL] = false;
        break;
    case VK_SHIFT:
        KeysDown[(unsigned)KeyCode::SHIFT] = false;
        break;
    case VK_MENU:
        KeysDown[(unsigned)KeyCode::ALT] = false;
        break;
    }
}

void InputSystem::HandleMousePositionMessage(unsigned x, unsigned y)
{
    mouseX = x;
    mouseY = y;
}

void InputSystem::HandleWindowsMessageLMB(bool mouseDown)
{
    KeysDown[(unsigned)KeyCode::LEFTMOUSE] = mouseDown;
}

void InputSystem::HandleWindowsMessageRMB(bool mouseDown)
{
    KeysDown[(unsigned)KeyCode::RIGHTMOUSE] = mouseDown;
}

void InputSystem::HandleMouseScrollMessage(int delta)
{
    mouseWheelDeltaPrevFrame = delta / WHEEL_DELTA;
}
