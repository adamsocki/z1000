//
// Created by Adam Socki on 6/2/25.
//


#include "entities/WallEntity.cpp"

void InitEntityHandleBuffers(GameData* gameData, MemoryArena* arena) {
    gameData->walls = MakeDynamicArray<EntityHandle>(arena, 10);

}

void InitGame(Zayn *zaynMem) {

    MeshCreationInfo m1 = {};
    m1.name = "viking_room";
    m1.path = GetModelPath("viking_room.obj");
    Mesh* mesh1 = MakeMesh(zaynMem, &m1);

    //Test Procedural Wall
    Mesh* wallMesh = CreateProceduralWall(zaynMem, 5.0f, 3.0f, 0.5f);
    Mesh* floorMesh = CreateProceduralPlane(zaynMem, 10.0f, 10.0f, 5);

    TextureCreateInfo texture1;
    texture1.path = "viking_room.png";
    texture1.name = "viking_texture";
    Texture* tex1 = MakeTexture(zaynMem, &texture1);


    MaterialCreateInfo material1;
    material1.type = MATERIAL_PBR;
    material1.texture = tex1;
    material1.name = "viking_material";
    Material* mat1 = MakeMaterial(zaynMem, &material1);


    InitEntityHandleBuffers(&zaynMem->gameData, &zaynMem->permanentMemory);

    // Example walls using new Euler angle rotation system (commented out for now):
    // North wall (no rotation)
    // EntityHandle wall_north = AddEntity(&zaynMem->entityFactory, EntityType_Wall);
    // PushBack(&zaynMem->gameData.walls, wall_north);
    // WallEntity* wallNorth = (WallEntity*)GetEntity(&zaynMem->entityFactory, wall_north);
    // wallNorth->material = mat1;
    // wallNorth->mesh = wallMesh;
    // mat4 northWallTransform = TRS(V3(0.0f, 0.0f, 0.0f), V3(0.0f, 0.0f, 0.0f), V3(1.0f, 1.0f, 1.0f));
    // AddMeshInstance(wallMesh, wall_north, northWallTransform);

    // // South wall (180 degrees Y rotation)
    // EntityHandle wall_south = AddEntity(&zaynMem->entityFactory, EntityType_Wall);
    // PushBack(&zaynMem->gameData.walls, wall_south);
    // WallEntity* wallSouth = (WallEntity*)GetEntity(&zaynMem->entityFactory, wall_south);
    // wallSouth->material = mat1;
    // wallSouth->mesh = mesh1;
    // mat4 southWallTransform = TRS(V3(0.0f, 1.0f, 15.0f), V3(0.0f, 180.0f, 0.0f), V3(10.0f, 3.0f, 1.0f));
    // AddMeshInstance(mesh1, wall_south, southWallTransform);
    //
    // // East wall (90 degrees Y rotation)
    // EntityHandle wall_east = AddEntity(&zaynMem->entityFactory, EntityType_Wall);
    // PushBack(&zaynMem->gameData.walls, wall_east);
    // WallEntity* wallEast = (WallEntity*)GetEntity(&zaynMem->entityFactory, wall_east);
    // wallEast->material = mat1;
    // wallEast->mesh = mesh1;
    // mat4 eastWallTransform = TRS(V3(15.0f, 1.0f, 0.0f), V3(0.0f, 90.0f, 0.0f), V3(10.0f, 3.0f, 1.0f));
    // AddMeshInstance(mesh1, wall_east, eastWallTransform);
    //
    // // West wall (-90 degrees Y rotation)
    // EntityHandle wall_west = AddEntity(&zaynMem->entityFactory, EntityType_Wall);
    // PushBack(&zaynMem->gameData.walls, wall_west);
    // WallEntity* wallWest = (WallEntity*)GetEntity(&zaynMem->entityFactory, wall_west);
    // wallWest->material = mat1;
    // wallWest->mesh = mesh1;
    // mat4 westWallTransform = TRS(V3(-15.0f, 1.0f, 0.0f), V3(0.0f, -90.0f, 0.0f), V3(10.0f, 3.0f, 1.0f));
    // AddMeshInstance(mesh1, wall_west, westWallTransform);

    std::cout << "Outdoor environment created:  4 perimeter walls of an instanced mesh" << std::endl;
    // EntityHandle wall_1 = AddEntity(&zaynMem->entityFactory, EntityType_Wall);
    // PushBack(&zaynMem->gameData.walls, wall_1);
    //
    // WallEntity* wallEntity = (WallEntity*)GetEntity(&zaynMem->entityFactory, wall_1);
    // wallEntity->material = mat1;
    // wallEntity->mesh = mesh1;
    // // AddComponents(&zaynMem->componentsFactory.componentsStorage.materialComponents, wall_1);
    // MaterialComponents* matComp = FindComponentsInArray(&zaynMem->componentsFactory.componentsStorage.materialComponents, wall_1);

    // EntityHandle newEntityHandle = {};
    // AddEntity(&zaynMem->entityFactory, &newEntityHandle, entityCreator.selectedEntityType);

    // entity->name = "default";
    // entity->material = &engine->materialFactory.materials[0];

}