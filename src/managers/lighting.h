//
// Created by Adam Socki on 6/10/25.
//

#define MAX_DYNAMIC_LIGHTS 32

enum LightType {
    DirectionalLight_Type,
    PointLight_Type,
    SpotLight_Type,
    
    LightType_Count,
};

struct SceneLighting {
    vec3 sunDirection;
    vec3 sunColor;
    real32 sunIntensity;
    vec3 ambientColor;
    real32 ambientIntensity;
};

struct DynamicLight {
    EntityHandle owner;
    LightType type;
    bool active;
    
    vec3 position;
    vec3 color;
    real32 intensity;
    
    // Point light attenuation
    real32 constant;
    real32 linear;
    real32 quadratic;
    
    // Spot light data
    vec3 direction;
    real32 innerConeAngle;
    real32 outerConeAngle;
};

struct LightingSystem {
    SceneLighting sceneLighting;
    DynamicLight dynamicLights[MAX_DYNAMIC_LIGHTS];
    uint32 activeDynamicLights;
};

// Function declarations
void InitLightingSystem(LightingSystem* lightingSystem);
void SetSceneLighting(LightingSystem* lightingSystem, vec3 sunDirection, vec3 sunColor, real32 sunIntensity, vec3 ambientColor, real32 ambientIntensity);
uint32 AddDynamicLight(LightingSystem* lightingSystem, LightType type, vec3 position, vec3 color, real32 intensity);
uint32 AddPointLight(LightingSystem* lightingSystem, vec3 position, vec3 color, real32 intensity, real32 constant, real32 linear, real32 quadratic);
uint32 AddSpotLight(LightingSystem* lightingSystem, vec3 position, vec3 direction, vec3 color, real32 intensity, real32 innerAngle, real32 outerAngle);
void RemoveDynamicLight(LightingSystem* lightingSystem, uint32 lightIndex);
void UpdateDynamicLightPosition(LightingSystem* lightingSystem, uint32 lightIndex, vec3 newPosition);
DynamicLight* GetDynamicLight(LightingSystem* lightingSystem, uint32 lightIndex);