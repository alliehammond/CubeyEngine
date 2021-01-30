#pragma once
#include "Core\CubeySystem.h"
#include <vector>

//Note: Use either number chars or capital letter chars to reference keypress ('1','A' for example)
//Note Key pressed/Key released will be true on the frame a key is either pressed or released
//      Key down will be true while the key is held down (and on the frame it is pressed or released)
class InputSystem : public CubeySystem
{
public:
    InputSystem();
    ~InputSystem();
    void Update(float dt);

    static bool GetKeyDown(char key);
    static bool GetKeyPressed(char key);
    static bool GetKeyReleased(char key);

    static void HandleWindowsMessageKeyDown(unsigned int code);
    static void HandleWindowsMessageKeyUp(unsigned int code);

private:
    static std::vector<bool> KeysDown;
    //Previous frame of keys down to calculate keys pressed and released
    static std::vector<bool> KeysDownPrev;
    static std::vector<bool> KeysPressed;
    static std::vector<bool> KeysReleased;
};
