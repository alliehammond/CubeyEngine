#include "EnginePCH.h"
#include "Core\InputSystem.h"
#include <vector>

std::vector<bool> InputSystem::KeysDown(36);
std::vector<bool> InputSystem::KeysDownPrev(36);
std::vector<bool> InputSystem::KeysPressed(36);
std::vector<bool> InputSystem::KeysReleased(36);

//Vector character layout:
//0-25 = 'A'-'Z'
//26-35 = '0'-'9'

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

    //Debug print key pressed
    /*for(int i = 0;i < KeysDown.size(); ++i)
    {
        if(KeysDown[i])LOGDEBUG(std::to_string(i) + " down!");
        if(KeysPressed[i])LOGDEBUG(std::to_string(i) + " pressed!");
        if(KeysReleased[i])LOGDEBUG(std::to_string(i) + " released!");
    }*/
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
}
