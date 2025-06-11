//
// Created by Level Editor
//

enum LightType {
    LightType_Directional,
    LightType_Point,
    LightType_Spot
};

struct LightEntity: Entity {
    vec3 position = V3(0, 0, 0);
    vec3 rotation = V3(0, 0, 0);
    
    LightType lightType = LightType_Point;
    vec3 color = V3(1, 1, 1);
    real32 intensity = 1.0f;
    real32 range = 10.0f;
    real32 spotAngle = 45.0f;
    
    bool castsShadows = true;
};