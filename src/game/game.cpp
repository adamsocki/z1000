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

    TextureCreateInfo texture1;
    texture1.path = "viking_room.png";
    texture1.name = "hi";
    MakeTexture(zaynMem, &texture1);
    // add to active entity list
    MaterialCreateInfo material1;
    material1.type = MATERIAL_PBR;
    material1.texture = &zaynMem->textureFactory.textures[0];
    material1.name = "mat1";
    AddComponents(&zaynMem->componentsFactory.componentsStorage.materialComponents, wall_1);
    MaterialComponents* matComp = FindComponentsInArray(&zaynMem->componentsFactory.componentsStorage.materialComponents, wall_1);

    // EntityHandle newEntityHandle = {};
    // AddEntity(&zaynMem->entityFactory, &newEntityHandle, entityCreator.selectedEntityType);

    Entity* entity = static_cast<Entity*>(GetEntity(&zaynMem->entityFactory, wall_1));


    // entity->name = "default";

    // entity->material = &engine->materialFactory.materials[0];

}