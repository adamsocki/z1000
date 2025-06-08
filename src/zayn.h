//
// Created by Adam Socki on 5/30/25.
//

#include "managers/memory.h"

#include "managers/time.h"
#include "dynamicArray.h"
#include "managers/window.h"
#include "managers/input.h"

#include "managers/camera.h"
#include "managers/render/render.h"
#include "managers/factory/entity_factory.h"
#include "managers/factory/components_factory.h"
#include "managers/factory/mesh_factory.h"
#include "managers/factory/texture_factory.h"
#include "managers/factory/material_factory.h"

#include "game/game.h"

struct Zayn {

    WindowManager windowManager;
    InputManager inputManager;
    Time time;
    Camera camera;

    MemoryArena frameMemory;
    MemoryArena permanentMemory;

    ComponentsFactory componentsFactory;
    EntityFactory entityFactory;
    MeshFactory meshFactory;
    MaterialFactory materialFactory;
    TextureFactory textureFactory;


    GameData gameData;

    Renderer renderer;



    EntityTypeInfoForBuffer entityTypeInfoForBuffer[EntityType_Count];
        // {EntityType_Player, sizeof(int32), 10, "Player"}
        // {EntityType_Floor, sizeof(FloorEntity), 10000, "Floor"},

        // {EntityType_Piano, sizeof(GrandPianoEntity), 10, "Grand Piano"}


};
