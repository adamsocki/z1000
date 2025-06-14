
#if IMGUI
#include "imgui_impl_glfw.h"
#endif

void AllocateInputManager(InputManager *inputManager, MemoryArena *arena, int32 deviceCapacity)
{
    // Use a reasonable chunk size (e.g., 64 or 128 events per chunk)
    const int EVENTS_PER_CHUNK = 128;  // Don't use deviceCapacity (5) as chunk size!
    inputManager->events = MakeDynamicArray<InputEvent>(arena, EVENTS_PER_CHUNK);

    inputManager->deviceCount = deviceCapacity;
    inputManager->devices = (InputDevice *)AllocateMem(arena, sizeof(InputDevice) * deviceCapacity);
    memset(inputManager->devices, 0, inputManager->deviceCount * sizeof(InputDevice));
}



InputKeyboardDiscrete glfwKeyToInputKeyboardDiscrete[GLFW_KEY_LAST + 1];


    void InitializeKeyMap()
    {
        for (int i = 0; i <= GLFW_KEY_LAST; ++i) {
            glfwKeyToInputKeyboardDiscrete[i] = Input_KeyboardDiscreteCount;
        }

        // Map GLFW keys to InputKeyboardDiscrete enum values
        glfwKeyToInputKeyboardDiscrete[GLFW_KEY_A] = Input_A;
        glfwKeyToInputKeyboardDiscrete[GLFW_KEY_B] = Input_B;
        glfwKeyToInputKeyboardDiscrete[GLFW_KEY_C] = Input_C;
        glfwKeyToInputKeyboardDiscrete[GLFW_KEY_D] = Input_D;
        glfwKeyToInputKeyboardDiscrete[GLFW_KEY_E] = Input_E;
        glfwKeyToInputKeyboardDiscrete[GLFW_KEY_F] = Input_F;
        glfwKeyToInputKeyboardDiscrete[GLFW_KEY_G] = Input_G;
        glfwKeyToInputKeyboardDiscrete[GLFW_KEY_H] = Input_H;
        glfwKeyToInputKeyboardDiscrete[GLFW_KEY_I] = Input_I;
        glfwKeyToInputKeyboardDiscrete[GLFW_KEY_J] = Input_J;
        glfwKeyToInputKeyboardDiscrete[GLFW_KEY_K] = Input_K;
        glfwKeyToInputKeyboardDiscrete[GLFW_KEY_L] = Input_L;
        glfwKeyToInputKeyboardDiscrete[GLFW_KEY_M] = Input_M;
        glfwKeyToInputKeyboardDiscrete[GLFW_KEY_N] = Input_N;
        glfwKeyToInputKeyboardDiscrete[GLFW_KEY_O] = Input_O;
        glfwKeyToInputKeyboardDiscrete[GLFW_KEY_P] = Input_P;
        glfwKeyToInputKeyboardDiscrete[GLFW_KEY_Q] = Input_Q;
        glfwKeyToInputKeyboardDiscrete[GLFW_KEY_R] = Input_R;
        glfwKeyToInputKeyboardDiscrete[GLFW_KEY_S] = Input_S;
        glfwKeyToInputKeyboardDiscrete[GLFW_KEY_T] = Input_T;
        glfwKeyToInputKeyboardDiscrete[GLFW_KEY_U] = Input_U;
        glfwKeyToInputKeyboardDiscrete[GLFW_KEY_V] = Input_V;
        glfwKeyToInputKeyboardDiscrete[GLFW_KEY_W] = Input_W;
        glfwKeyToInputKeyboardDiscrete[GLFW_KEY_X] = Input_X;
        glfwKeyToInputKeyboardDiscrete[GLFW_KEY_Y] = Input_Y;
        glfwKeyToInputKeyboardDiscrete[GLFW_KEY_Z] = Input_Z;
        glfwKeyToInputKeyboardDiscrete[GLFW_KEY_0] = Input_0;
        glfwKeyToInputKeyboardDiscrete[GLFW_KEY_1] = Input_1;
        glfwKeyToInputKeyboardDiscrete[GLFW_KEY_2] = Input_2;
        glfwKeyToInputKeyboardDiscrete[GLFW_KEY_3] = Input_3;
        glfwKeyToInputKeyboardDiscrete[GLFW_KEY_4] = Input_4;
        glfwKeyToInputKeyboardDiscrete[GLFW_KEY_5] = Input_5;
        glfwKeyToInputKeyboardDiscrete[GLFW_KEY_6] = Input_6;
        glfwKeyToInputKeyboardDiscrete[GLFW_KEY_7] = Input_7;
        glfwKeyToInputKeyboardDiscrete[GLFW_KEY_8] = Input_8;
        glfwKeyToInputKeyboardDiscrete[GLFW_KEY_9] = Input_9;
        glfwKeyToInputKeyboardDiscrete[GLFW_KEY_EQUAL] = Input_Equal;
        glfwKeyToInputKeyboardDiscrete[GLFW_KEY_MINUS] = Input_Minus;
        glfwKeyToInputKeyboardDiscrete[GLFW_KEY_RIGHT_BRACKET] = Input_RightBracket;
        glfwKeyToInputKeyboardDiscrete[GLFW_KEY_LEFT_BRACKET] = Input_LeftBracket;
        glfwKeyToInputKeyboardDiscrete[GLFW_KEY_APOSTROPHE] = Input_Quote;
        glfwKeyToInputKeyboardDiscrete[GLFW_KEY_SEMICOLON] = Input_Semicolon;
        glfwKeyToInputKeyboardDiscrete[GLFW_KEY_BACKSLASH] = Input_Backslash;
        glfwKeyToInputKeyboardDiscrete[GLFW_KEY_COMMA] = Input_Comma;
        glfwKeyToInputKeyboardDiscrete[GLFW_KEY_SLASH] = Input_Slash;
        glfwKeyToInputKeyboardDiscrete[GLFW_KEY_PERIOD] = Input_Period;
        glfwKeyToInputKeyboardDiscrete[GLFW_KEY_ENTER] = Input_Return;
        glfwKeyToInputKeyboardDiscrete[GLFW_KEY_TAB] = Input_Tab;
        glfwKeyToInputKeyboardDiscrete[GLFW_KEY_SPACE] = Input_Space;
        glfwKeyToInputKeyboardDiscrete[GLFW_KEY_BACKSPACE] = Input_Backspace;
        glfwKeyToInputKeyboardDiscrete[GLFW_KEY_ESCAPE] = Input_Escape;
        glfwKeyToInputKeyboardDiscrete[GLFW_KEY_GRAVE_ACCENT] = Input_Tick;
        glfwKeyToInputKeyboardDiscrete[GLFW_KEY_LEFT_SUPER] = Input_Win;
        glfwKeyToInputKeyboardDiscrete[GLFW_KEY_LEFT_SHIFT] = Input_Shift;
        glfwKeyToInputKeyboardDiscrete[GLFW_KEY_CAPS_LOCK] = Input_CapsLock;
        glfwKeyToInputKeyboardDiscrete[GLFW_KEY_LEFT_ALT] = Input_Alt;
        glfwKeyToInputKeyboardDiscrete[GLFW_KEY_LEFT_CONTROL] = Input_Control;
        glfwKeyToInputKeyboardDiscrete[GLFW_KEY_RIGHT_SUPER] = Input_RightWin;
        glfwKeyToInputKeyboardDiscrete[GLFW_KEY_RIGHT_SHIFT] = Input_RightShift;
        glfwKeyToInputKeyboardDiscrete[GLFW_KEY_RIGHT_ALT] = Input_RightAlt;
        glfwKeyToInputKeyboardDiscrete[GLFW_KEY_RIGHT_CONTROL] = Input_RightControl;
        glfwKeyToInputKeyboardDiscrete[GLFW_KEY_F1] = Input_F1;
        glfwKeyToInputKeyboardDiscrete[GLFW_KEY_F2] = Input_F2;
        glfwKeyToInputKeyboardDiscrete[GLFW_KEY_F3] = Input_F3;
        glfwKeyToInputKeyboardDiscrete[GLFW_KEY_F4] = Input_F4;
        glfwKeyToInputKeyboardDiscrete[GLFW_KEY_F5] = Input_F5;
        glfwKeyToInputKeyboardDiscrete[GLFW_KEY_F6] = Input_F6;
        glfwKeyToInputKeyboardDiscrete[GLFW_KEY_F7] = Input_F7;
        glfwKeyToInputKeyboardDiscrete[GLFW_KEY_F8] = Input_F8;
        glfwKeyToInputKeyboardDiscrete[GLFW_KEY_F9] = Input_F9;
        glfwKeyToInputKeyboardDiscrete[GLFW_KEY_F10] = Input_F10;
        glfwKeyToInputKeyboardDiscrete[GLFW_KEY_F11] = Input_F11;
        glfwKeyToInputKeyboardDiscrete[GLFW_KEY_F12] = Input_F12;
        glfwKeyToInputKeyboardDiscrete[GLFW_KEY_F13] = Input_F13;
        glfwKeyToInputKeyboardDiscrete[GLFW_KEY_F14] = Input_F14;
        glfwKeyToInputKeyboardDiscrete[GLFW_KEY_F15] = Input_F15;
        glfwKeyToInputKeyboardDiscrete[GLFW_KEY_F16] = Input_F16;
        glfwKeyToInputKeyboardDiscrete[GLFW_KEY_F17] = Input_F17;
        glfwKeyToInputKeyboardDiscrete[GLFW_KEY_F18] = Input_F18;
        glfwKeyToInputKeyboardDiscrete[GLFW_KEY_F19] = Input_F19;
        glfwKeyToInputKeyboardDiscrete[GLFW_KEY_F20] = Input_F20;
        glfwKeyToInputKeyboardDiscrete[GLFW_KEY_F21] = Input_F21;
        glfwKeyToInputKeyboardDiscrete[GLFW_KEY_F22] = Input_F22;
        glfwKeyToInputKeyboardDiscrete[GLFW_KEY_F23] = Input_F23;
        glfwKeyToInputKeyboardDiscrete[GLFW_KEY_F24] = Input_F24;
        glfwKeyToInputKeyboardDiscrete[GLFW_KEY_HOME] = Input_Home;
        glfwKeyToInputKeyboardDiscrete[GLFW_KEY_INSERT] = Input_Insert;
        glfwKeyToInputKeyboardDiscrete[GLFW_KEY_PAGE_UP] = Input_PageUp;
        glfwKeyToInputKeyboardDiscrete[GLFW_KEY_DELETE] = Input_ForwardDelete;
        glfwKeyToInputKeyboardDiscrete[GLFW_KEY_END] = Input_End;
        glfwKeyToInputKeyboardDiscrete[GLFW_KEY_PAGE_DOWN] = Input_PageDown;
        glfwKeyToInputKeyboardDiscrete[GLFW_KEY_LEFT] = Input_LeftArrow;
        glfwKeyToInputKeyboardDiscrete[GLFW_KEY_RIGHT] = Input_RightArrow;
        glfwKeyToInputKeyboardDiscrete[GLFW_KEY_DOWN] = Input_DownArrow;
        glfwKeyToInputKeyboardDiscrete[GLFW_KEY_UP] = Input_UpArrow;
    }

void AllocateInputDevice(InputDevice *device, InputDeviceType type,
                        int32 discreteCount, int32 analogueCount, MemoryArena* arena)
    {
        device->type = type;
        device->discreteCount = discreteCount;
        device->analogueCount = analogueCount;

        // Use the arena allocator instead of malloc
        if (discreteCount > 0) {
            device->framesHeld = (int32*)AllocateMem(arena, sizeof(int32) * discreteCount);
            device->released = (bool*)AllocateMem(arena, sizeof(bool) * discreteCount);
            device->pressed = (bool*)AllocateMem(arena, sizeof(bool) * discreteCount);
            device->timePressed = (real32*)AllocateMem(arena, sizeof(real32) * discreteCount);

            memset(device->framesHeld, -1, sizeof(int32) * discreteCount);
            memset(device->pressed, 0, sizeof(bool) * discreteCount);
            memset(device->released, 0, sizeof(bool) * discreteCount);
            memset(device->timePressed, 0, sizeof(real32) * discreteCount);
        }

        if (analogueCount > 0) {
            device->prevAnalogue = (real32*)AllocateMem(arena, sizeof(real32) * analogueCount);
            device->analogue = (real32*)AllocateMem(arena, sizeof(real32) * analogueCount);

            memset(device->analogue, 0, sizeof(real32) * analogueCount);
            memset(device->prevAnalogue, 0, sizeof(real32) * analogueCount);
        }
    }


void InitInputManager(InputManager *inputManager, MemoryArena* permanentMemory) {

    // InputManager* inputManager = im;
    // MemoryArena* permanentMemory = permMem;

    std::cout << "initInputManager: Done" << '\n';

    AllocateInputManager(inputManager, permanentMemory, 5);
    // inputManager->windowReference = *windowReference;/**/

    inputManager->keyboard = &inputManager->devices[0];
    inputManager->mouse = &inputManager->devices[1];
        AllocateInputDevice(inputManager->keyboard, InputDeviceType_Keyboard,
                          Input_KeyboardDiscreteCount, 0, permanentMemory);
        AllocateInputDevice(inputManager->mouse, InputDeviceType_Mouse,
                           Input_MouseDiscreteCount, Input_MouseAnalogueCount, permanentMemory);

    InitializeKeyMap();
}

bool InputHeld(InputDevice* device, int32 inputID)
{
    return device->framesHeld[inputID] > 0;
}

bool InputPressed(InputDevice *device, int32 inputID)
{
    return device->pressed[inputID] && device->framesHeld[inputID] == 0;
}
void mouseButtonCallback(GLFWwindow* window2, int button, int action, int mods)
    {


#if IMGUI
        ImGui_ImplGlfw_MouseButtonCallback(window2, button, action, mods);
#endif
    }


void cursorPosCallback(GLFWwindow* window, double xpos, double ypos)
    {
        InputManager* inputManager = static_cast<InputManager*>(glfwGetWindowUserPointer(window));
        inputManager->lastMousePosWindow = inputManager->nowMousePosWindow;
        inputManager->mousePosUpdated = true;
        inputManager->nowMousePosWindow.x = xpos;
        inputManager->nowMousePosWindow.y = ypos;

        inputManager->mousePos.x = (int32)xpos;
        inputManager->mousePos.y = (int32)ypos;

        real32 xOffset = (real32)(xpos - inputManager->lastMousePosWindow.x);
        real32 yOffset = (real32)(ypos - inputManager->lastMousePosWindow.y);

        inputManager->mouse->analogue[Input_MousePositionX] = (real32)xpos;
        inputManager->mouse->analogue[Input_MousePositionY] = (real32)ypos;
        inputManager->mouse->analogue[Input_MousePositionXOffset] = xOffset;
        inputManager->mouse->analogue[Input_MousePositionYOffset] = yOffset;


        InputEvent eventX = {};
        eventX.device = inputManager->mouse;
        eventX.index = Input_MousePositionX;
        eventX.value = (real32)xpos;

        InputEvent eventY = {};
        eventY.device = inputManager->mouse;
        eventY.index = Input_MousePositionY;
        eventY.value = (real32)ypos;

        InputEvent eventXOffset = {};
        eventXOffset.device = inputManager->mouse;
        eventXOffset.index = Input_MousePositionXOffset;
        eventXOffset.value = (real32)(xpos - inputManager->lastMousePosWindow.x);

        InputEvent eventYOffset = {};
        eventYOffset.device = inputManager->mouse;
        eventYOffset.index = Input_MousePositionYOffset;
        eventYOffset.value = (real32)(ypos - inputManager->lastMousePosWindow.y);

        PushBack(&inputManager->events, eventX);
        PushBack(&inputManager->events, eventY);
        PushBack(&inputManager->events, eventXOffset);
        PushBack(&inputManager->events, eventYOffset);


//        std::cout << "X Pos: " << xpos << std::endl;
//        std::cout << "Y Pos: " << ypos << std::endl;


        // std::cout << "Key pressed: " << xpos << std::endl;
#if IMGUI
        ImGui_ImplGlfw_CursorPosCallback(window, xpos, ypos);
#endif

    }

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        InputManager* inputManager = static_cast<InputManager*>(glfwGetWindowUserPointer(window));

        // Add bounds checking for GLFW key
        if (key < 0 || key > GLFW_KEY_LAST)
        {
            // Invalid key, ignore
            return;
        }

        InputKeyboardDiscrete inputKey = glfwKeyToInputKeyboardDiscrete[key];

        // Check if the key is mapped
        if (inputKey >= Input_KeyboardDiscreteCount)
        {
            // Unmapped key, ignore
            return;
        }

        if (action == GLFW_PRESS)
        {
            InputEvent event = {};
            event.device = inputManager->keyboard;
            event.index = inputKey;
            event.discreteValue = true;
            PushBack(&inputManager->events, event);
        }
        else if (action == GLFW_RELEASE)
        {
            InputEvent event = {};
            event.device = inputManager->keyboard;
            event.index = inputKey;
            event.discreteValue = false;
            PushBack(&inputManager->events, event);
        }

#if IMGUI
        ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);
#endif
    }

void UpdateInputManager(Zayn* zaynMem) 
{
        InputManager* inputManager = &zaynMem->inputManager;
    WindowManager* windowManager = &zaynMem->windowManager;
    Time* time = &zaynMem->time;

    // Clear events from previous frame FIRST
    DynamicArrayClear(&inputManager->events);

    glfwPollEvents();

    // Set callbacks only once (move this to initialization)
    static bool callbacksSet = false;
    if (!callbacksSet) {
        glfwSetKeyCallback(windowManager->glfwWindow, keyCallback);
        glfwSetMouseButtonCallback(windowManager->glfwWindow, mouseButtonCallback);
        glfwSetCursorPosCallback(windowManager->glfwWindow, cursorPosCallback);
        callbacksSet = true;
    }

    // Process discrete inputs
    for (int d = 0; d < inputManager->deviceCount; d++)
    {
        InputDevice* device = &inputManager->devices[d];
        if (device->type == InputDeviceType_Invalid) continue;

        for (int i = 0; i < device->discreteCount; i++)
        {
            device->released[i] = false;
            if (device->framesHeld[i] >= 0)
            {
                device->framesHeld[i]++;
                device->pressed[i] = false;
            }
        }
    }

    // Process events with bounds checking
    for (int i = 0; i < inputManager->events.count; i++)
    {
        InputEvent event = inputManager->events[i];
        int32 index = event.index;
        InputDevice* device = event.device;

        // Validate index bounds
        if (index < 0 || index >= device->discreteCount) {
            continue;  // Skip invalid event
        }

        if (!event.discreteValue)
        {
            if (device->framesHeld[index] > 0)
            {
                device->released[index] = true;
            }
            device->timePressed[index] = -1;
            device->framesHeld[index] = -1;
            device->pressed[index] = false;
        }
        else
        {
            if (device->framesHeld[index] < 0)
            {
                device->timePressed[index] = time->totalTime;
                device->framesHeld[index] = 0;
                device->pressed[index] = true;
                device->released[index] = false;
            }
        }
    }

    // Update mouse position
    if (inputManager->mousePosUpdated)
    {
        int width, height;
        glfwGetWindowSize(windowManager->glfwWindow, &width, &height);

        if (width > 0 && height > 0) {  // Prevent division by zero
            inputManager->mousePosNorm.x = inputManager->nowMousePosWindow.x / (real32)width;
            inputManager->mousePosNorm.y = inputManager->nowMousePosWindow.y / (real32)height;
            inputManager->mousePosNormSigned.x = (inputManager->mousePosNorm.x * 2.0f) - 1.0f;
            inputManager->mousePosNormSigned.y = (inputManager->mousePosNorm.y * 2.0f) - 1.0f;
        }

        inputManager->mouse->analogue[Input_MousePositionXNorm] = inputManager->mousePosNorm.x;
        inputManager->mouse->analogue[Input_MousePositionYNorm] = inputManager->mousePosNorm.y;
        inputManager->mouse->analogue[Input_MousePositionX] = inputManager->nowMousePosWindow.x;
        inputManager->mouse->analogue[Input_MousePositionY] = inputManager->nowMousePosWindow.y;

        inputManager->mousePosUpdated = false;
        inputManager->lastMousePosWindow = inputManager->nowMousePosWindow;
    }

    // Clear mouse offsets for next frame
    inputManager->mouse->analogue[Input_MousePositionXOffset] = 0.0f;
    inputManager->mouse->analogue[Input_MousePositionYOffset] = 0.0f;
}

void ClearInputManager(Zayn* zaynMem) 
{
    InputManager* inputManager = &zaynMem->inputManager;

    DynamicArrayClear(&inputManager->events);

    inputManager->mouse->analogue[Input_MousePositionXOffset] = 0.0f;
    inputManager->mouse->analogue[Input_MousePositionYOffset] = 0.0f;


    inputManager->charCount = 0;
    memset(inputManager->inputChars, 0, inputManager->charSize);
}




