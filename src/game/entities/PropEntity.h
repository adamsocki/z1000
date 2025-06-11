//
// Created by Level Editor
//

struct PropEntity: Entity {
    vec3 position = V3(0, 0, 0);
    vec3 rotation = V3(0, 0, 0);
    vec3 scale = V3(1, 1, 1);
    
    Mesh* mesh;
    Material* material;
    
    bool isStatic = true;
    bool castsShadows = true;
    real32 collisionRadius = 1.0f;
};