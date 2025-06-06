//
// Created by Adam Socki on 5/30/25.
//

#define VULKAN 1
#define OPENGL 0
#define METAL  0

#include "DataTypes.h"
#include "math/math.h"

#include "zayn.h"

#include <iostream>
#include <optional>
#include "managers/memory.cpp"
#include "managers/time.cpp"
#include "managers/window.cpp"
#include "managers/input.cpp"
#include "managers/camera.cpp"


#include "managers/render/render.cpp"
#include "managers/factory/components_factory.cpp"
#include "managers/factory/mesh_factory.cpp"
#include "managers/factory/material_factory.cpp"
#include "managers/factory/entity_factory.cpp"

#include "game/game.cpp"








void InitZayn(Zayn* zaynMem) {

    std::cout<<"InitEngine"<<std::endl;

    // @todo: create init log

    AllocateMemoryArena(&zaynMem->frameMemory, Megabytes(32));
    AllocateMemoryArena(&zaynMem->permanentMemory, Megabytes(32));

    InitTime(&zaynMem->time);
    InitInputManager(&zaynMem->inputManager, &zaynMem->permanentMemory);
    InitWindow(&zaynMem->windowManager, &zaynMem->inputManager, V2(600, 400), "mac zayn");

    InitCamera(&zaynMem->camera, zaynMem->windowManager.glfwWindow, &zaynMem->inputManager);

    InitMeshFactory(&zaynMem->meshFactory, &zaynMem->permanentMemory);

    InitEntityFactory(&zaynMem->entityFactory, zaynMem);
    InitComponentsFactory(&zaynMem->componentsFactory, &zaynMem->permanentMemory);

    InitRender(zaynMem);
    InitGame(zaynMem);

};


void UpdateZayn(Zayn* zaynMem) {

    UpdateTime(zaynMem);
    UpdateInputManager(zaynMem);

    UpdateCamera(&zaynMem->windowManager, &zaynMem->camera, &zaynMem->inputManager, &zaynMem->time);


    // UpdateComponentsFactory(Z);



    UpdateRenderer(zaynMem, &zaynMem->renderer, &zaynMem->windowManager, &zaynMem->camera, &zaynMem->inputManager);

    

    // LOGIC
    if (InputHeld(zaynMem->inputManager.keyboard, Input_A)) {
        std::cout<<"A is held"<<std::endl;
    }
    if (InputHeld(zaynMem->inputManager.keyboard, Input_B)) {
        std::cout<<"B is held"<<std::endl;
    }
    if (InputHeld(zaynMem->inputManager.keyboard, Input_C)) {
        std::cout<<"C is held"<<std::endl;
    }

    if (InputPressed(zaynMem->inputManager.keyboard, Input_Escape)) {
        // close the window
        std::cout<<"Escape is pressed"<<std::endl;
        glfwSetWindowShouldClose(zaynMem->windowManager.glfwWindow, true);
    }


    ClearInputManager(zaynMem);
}
void ShutdownZayn(Zayn* zaynMem) {
    std::cout<<"ShutdownEngine"<<std::endl;
    glfwTerminate();
}

// void InitEngine(Engine* engine) {
//
// };


