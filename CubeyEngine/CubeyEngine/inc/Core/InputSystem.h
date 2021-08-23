#pragma once
#include "Core\CubeySystem.h"
#include <vector>

enum class KeyCode : unsigned char
{
    CTRL = 36,
    SHIFT,
    ALT,
    LEFTMOUSE,
    RIGHTMOUSE,
    NUMCODES
};

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
    static bool GetKeyDown(KeyCode key);
    static bool GetKeyPressed(KeyCode key);
    static bool GetKeyReleased(KeyCode key);
    static unsigned GetMouseX() { return mouseX; }
    static unsigned GetMouseY() { return mouseY; }
    static int GetMouseDeltaX() { return mouseDeltaX; }
    static int GetMouseDeltaY() { return mouseDeltaY; }
    static int GetMouseWheelDelta() { return mouseWheelDelta; }

    static void HandleWindowsMessageKeyDown(unsigned int code);
    static void HandleWindowsMessageKeyUp(unsigned int code);
    static void HandleMousePositionMessage(unsigned x, unsigned y);
    static void HandleMouseScrollMessage(int delta);
    //mouseDown false indicated mouse up message
    static void HandleWindowsMessageLMB(bool mouseDown);
    static void HandleWindowsMessageRMB(bool mouseDown);

private:
    static std::vector<bool> KeysDown;
    //Previous frame of keys down to calculate keys pressed and released
    static std::vector<bool> KeysDownPrev;
    static std::vector<bool> KeysPressed;
    static std::vector<bool> KeysReleased;

    //Mouse x, y top left corner is (0, 0)
    static unsigned mouseX, mouseY;
    //Mouse delta x positive = right, delta y positive = up
    static int mouseDeltaX, mouseDeltaY;
    //Scroll wheel delta, positive away from user, negative towards
    static int mouseWheelDelta, mouseWheelDeltaPrevFrame;
};
