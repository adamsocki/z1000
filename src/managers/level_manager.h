//
// Created by Level Manager
//

#define MAX_LEVEL_ENTITIES 1000

struct LevelEntity {
    char typeName[32];
    uint32 id;
    
    // Transform
    vec3 position;
    vec3 rotation;
    vec3 scale;
    
    // Rendering
    char meshName[64];
    char materialName[64];
    
    // Props
    bool isStatic;
    bool castsShadows;
    real32 collisionRadius;
    
    // Lights
    char lightTypeName[16];
    vec3 color;
    real32 intensity;
    real32 range;
    real32 spotAngle;
};

struct LevelData {
    char levelName[64];
    char version[16];
    
    LevelEntity entities[MAX_LEVEL_ENTITIES];
    uint32 entityCount;
};

struct LevelManager {
    LevelData currentLevel;
    bool isLevelLoaded;
};