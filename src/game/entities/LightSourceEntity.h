//
// Created by Simple Lighting System
//

struct LightSourceEntity: Entity {
    vec3 position = V3(0, 0, 0);
    vec3 color = V3(1, 1, 1);  // White light by default
    
    // Visual representation (optional - for debugging/editor)
    Mesh* mesh = nullptr;
    Material* material = nullptr;
};