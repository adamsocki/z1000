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
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"
#include "managers/memory.cpp"
#include "managers/time.cpp"
#include "managers/window.cpp"
#include "managers/input.cpp"
#include "managers/camera.cpp"


#include "managers/factory/entity_factory.cpp"
#include "managers/render/render.cpp"
#include "managers/factory/components_factory.cpp"
#include "managers/factory/mesh_factory.cpp"
#include "managers/factory/material_factory.cpp"
#include "managers/factory/texture_factory.cpp"
#include "managers/level_manager.cpp"
#include "managers/level_editor.cpp"

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
    InitTextureFactory(zaynMem);
    InitMaterialFactory(zaynMem);
    InitEntityFactory(&zaynMem->entityFactory, zaynMem);
    InitComponentsFactory(&zaynMem->componentsFactory, &zaynMem->permanentMemory);
    InitLevelManager(&zaynMem->levelManager);
    InitLevelEditor(&zaynMem->levelEditor);

    InitRender(zaynMem);
    CreateBasicLightingMaterials(zaynMem);  // Create colored materials after Vulkan is initialized
    InitGame(zaynMem);

};


void UpdateZayn(Zayn* zaynMem) {

    UpdateTime(zaynMem);
    UpdateInputManager(zaynMem);

    UpdateCamera(&zaynMem->windowManager, &zaynMem->camera, &zaynMem->inputManager, &zaynMem->time);

    // Update level editor
    UpdateLevelEditor(zaynMem, &zaynMem->levelEditor);

    UpdateRenderer(zaynMem, &zaynMem->renderer, &zaynMem->windowManager, &zaynMem->camera, &zaynMem->inputManager);

    // LOGIC

    if (InputPressed(zaynMem->inputManager.keyboard, Input_Escape)) {
        // close the window
        std::cout<<"Escape is pressed"<<std::endl;
        glfwSetWindowShouldClose(zaynMem->windowManager.glfwWindow, true);
    }
    
    // Level editor shortcuts
    if (InputPressed(zaynMem->inputManager.keyboard, Input_L)) {
        std::cout << "Loading test level..." << std::endl;
        LoadLevel(zaynMem, "test_level.json");
    }
    if (InputPressed(zaynMem->inputManager.keyboard, Input_P)) {
        std::cout << "Saving current level..." << std::endl;
        SaveLevel(zaynMem, "saved_level.json");
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


