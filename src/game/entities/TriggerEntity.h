//
// Created by Level Editor
//

enum TriggerType {
    TriggerType_Box,
    TriggerType_Sphere,
    TriggerType_Cylinder
};

struct TriggerEntity: Entity {
    vec3 position = V3(0, 0, 0);
    vec3 rotation = V3(0, 0, 0);
    vec3 scale = V3(1, 1, 1);
    
    TriggerType triggerType = TriggerType_Box;
    bool isVisible = false;
    int32 triggerId = 0;
    
    // Visual representation for editor
    vec3 debugColor = V3(0, 1, 0);
};