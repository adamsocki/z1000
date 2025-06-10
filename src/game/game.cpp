//
// Created by Adam Socki on 6/2/25.
//


#include "entities/WallEntity.cpp"

void InitEntityHandleBuffers(GameData* gameData, MemoryArena* arena) {
    gameData->walls = MakeDynamicArray<EntityHandle>(arena, 10);

}

void InitGame(Zayn *zaynMem) {

    // InitEntityHandleBuffers(&zaynMem->gameData, &zaynMem->permanentMemory);
    //
    // EntityHandle wall_1 = AddEntity(&zaynMem->entityFactory, EntityType_Wall);
    // PushBack(&zaynMem->gameData.walls, wall_1);


    // AddComponents(&zaynMem->componentsFactory.componentsStorage.meshComponents, wall_1);
    // MeshComponents* mc = FindComponentsInArray(&zaynMem->componentsFactory.componentsStorage.meshComponents, wall_1);
    // AddMeshToMeshComponent();
    MeshCreationInfo m1 = {};
    m1.name = "vk1";
    m1.path = GetModelPath("viking_room.obj");
    Mesh* mesh1 = MakeMesh(zaynMem, &m1);
    TextureCreateInfo texture1;
    texture1.path = "viking_room.png";
    texture1.name = "hi";
    Texture* tex1 = MakeTexture(zaynMem, &texture1);
    // add to active entity list
    MaterialCreateInfo material1;
    material1.type = MATERIAL_PBR;
    material1.texture = tex1;
    material1.name = "mat1";
    Material* mat1 = MakeMaterial(zaynMem, &material1);


    InitEntityHandleBuffers(&zaynMem->gameData, &zaynMem->permanentMemory);
    EntityHandle wall_1 = AddEntity(&zaynMem->entityFactory, EntityType_Wall);
    PushBack(&zaynMem->gameData.walls, wall_1);

    WallEntity* wallEntity = (WallEntity*)GetEntity(&zaynMem->entityFactory, wall_1);
    wallEntity->material = mat1;
    wallEntity->mesh = mesh1;
    // AddComponents(&zaynMem->componentsFactory.componentsStorage.materialComponents, wall_1);
    // MaterialComponents* matComp = FindComponentsInArray(&zaynMem->componentsFactory.componentsStorage.materialComponents, wall_1);

    // EntityHandle newEntityHandle = {};
    // AddEntity(&zaynMem->entityFactory, &newEntityHandle, entityCreator.selectedEntityType);

    // entity->name = "default";
    // entity->material = &engine->materialFactory.materials[0];

}