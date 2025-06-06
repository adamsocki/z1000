//
// Created by Adam Socki on 6/2/25.
//


#include "entities/WallEntity.cpp"

void InitEntityHandleBuffers(GameData* gameData, MemoryArena* arena) {
    gameData->walls = MakeDynamicArray<EntityHandle>(arena, 10);

}

void InitGame(Zayn *zaynMem) {

    InitEntityHandleBuffers(&zaynMem->gameData, &zaynMem->permanentMemory);
    // Make Game Chare
    EntityHandle wall_1 = AddEntity(&zaynMem->entityFactory, EntityType_Wall);
    PushBack(&zaynMem->gameData.walls, wall_1);
    // AddTransformToEntity();
    AddComponents(&zaynMem->componentsFactory.componentsStorage.transformComponents, wall_1);
    // AddMeshToEntity();

    AddComponents(&zaynMem->componentsFactory.componentsStorage.meshComponents, wall_1);
    TransformComponents* tc = FindComponentsInArray(&zaynMem->componentsFactory.componentsStorage.transformComponents, wall_1);

    AddComponents(&zaynMem->componentsFactory.componentsStorage.meshComponents, wall_1);
    MeshComponents* mc = FindComponentsInArray(&zaynMem->componentsFactory.componentsStorage.meshComponents, wall_1);
    // AddMeshToMeshComponent();
    MeshCreationInfo m1 = {};
    m1.name = "vk1";
    m1.path = GetModelPath("viking_room.obj");
    MakeMesh(zaynMem, &m1);

    PushBack(&zaynMem->entityFactory.activeEntityHandles, wall_1);

    // add to active entity list





}