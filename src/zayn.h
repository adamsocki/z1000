//
// Created by Adam Socki on 5/30/25.
//

#include "managers/memory.h"

#include "managers/time.h"
#include "dynamicArray.h"
#include "managers/window.h"
#include "managers/input.h"

#include "managers/camera.h"
#include "managers/factory/entity_factory.h"
#include "managers/render/render.h"

struct Zayn {

    WindowManager windowManager;
    InputManager inputManager;
    Time time;
    Camera camera;

    MemoryArena frameMemory;
    MemoryArena permanentMemory;
    
    EntityFactory entityFactory;

    Renderer renderer;
};
