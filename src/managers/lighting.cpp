//
// Created by Adam Socki on 6/10/25.
//

void InitLightingSystem(LightingSystem* lightingSystem) {
    if (!lightingSystem) return;
    
    // Initialize scene lighting with balanced values
    lightingSystem->sceneLighting.sunDirection = V3(0.3f, -0.8f, -0.5f); // Angled sun
    lightingSystem->sceneLighting.sunColor = V3(1.0f, 1.0f, 0.9f);
    lightingSystem->sceneLighting.sunIntensity = 0.4f; // Moderate sun
    lightingSystem->sceneLighting.ambientColor = V3(0.1f, 0.1f, 0.15f); // Soft blue ambient
    lightingSystem->sceneLighting.ambientIntensity = 0.2f;
    
    // Initialize dynamic lights
    lightingSystem->activeDynamicLights = 0;
    for (uint32 i = 0; i < MAX_DYNAMIC_LIGHTS; i++) {
        lightingSystem->dynamicLights[i].active = false;
        lightingSystem->dynamicLights[i].owner.indexInInfo = 0;
        lightingSystem->dynamicLights[i].owner.generation = 0;
        lightingSystem->dynamicLights[i].owner.type = EntityType_Player; // Use valid enum value
    }
}

void SetSceneLighting(LightingSystem* lightingSystem, vec3 sunDirection, vec3 sunColor, real32 sunIntensity, vec3 ambientColor, real32 ambientIntensity) {
    if (!lightingSystem) return;
    
    lightingSystem->sceneLighting.sunDirection = Normalize(sunDirection);
    lightingSystem->sceneLighting.sunColor = sunColor;
    lightingSystem->sceneLighting.sunIntensity = sunIntensity;
    lightingSystem->sceneLighting.ambientColor = ambientColor;
    lightingSystem->sceneLighting.ambientIntensity = ambientIntensity;
}

uint32 AddDynamicLight(LightingSystem* lightingSystem, LightType type, vec3 position, vec3 color, real32 intensity) {
    if (!lightingSystem || lightingSystem->activeDynamicLights >= MAX_DYNAMIC_LIGHTS) {
        printf("ERROR: Failed to add light - system null or too many lights!\n");
        return UINT32_MAX; // Invalid index
    }
    
    uint32 index = lightingSystem->activeDynamicLights;
    DynamicLight* light = &lightingSystem->dynamicLights[index];
    
    light->active = true;
    light->type = type;
    light->position = position;
    light->color = color;
    light->intensity = intensity;
    
    // Set default attenuation for point lights
    light->constant = 1.0f;
    light->linear = 0.09f;
    light->quadratic = 0.032f;
    
    // Default spot light values
    light->direction = V3(0.0f, -1.0f, 0.0f);
    light->innerConeAngle = 12.5f;
    light->outerConeAngle = 15.0f;
    
    lightingSystem->activeDynamicLights++;
    
    printf("Added dynamic light %d: type=%d, pos=(%.1f,%.1f,%.1f), color=(%.1f,%.1f,%.1f), intensity=%.1f, total lights=%d\n", 
           index, type, position.x, position.y, position.z, color.x, color.y, color.z, intensity, lightingSystem->activeDynamicLights);
    
    return index;
}

uint32 AddPointLight(LightingSystem* lightingSystem, vec3 position, vec3 color, real32 intensity, real32 constant, real32 linear, real32 quadratic) {
    uint32 index = AddDynamicLight(lightingSystem, PointLight_Type, position, color, intensity);
    if (index != UINT32_MAX) {
        DynamicLight* light = &lightingSystem->dynamicLights[index];
        light->constant = constant;
        light->linear = linear;
        light->quadratic = quadratic;
    }
    return index;
}

uint32 AddSpotLight(LightingSystem* lightingSystem, vec3 position, vec3 direction, vec3 color, real32 intensity, real32 innerAngle, real32 outerAngle) {
    uint32 index = AddDynamicLight(lightingSystem, SpotLight_Type, position, color, intensity);
    if (index != UINT32_MAX) {
        DynamicLight* light = &lightingSystem->dynamicLights[index];
        light->direction = Normalize(direction);
        light->innerConeAngle = innerAngle;
        light->outerConeAngle = outerAngle;
    }
    return index;
}

void RemoveDynamicLight(LightingSystem* lightingSystem, uint32 lightIndex) {
    if (!lightingSystem || lightIndex >= lightingSystem->activeDynamicLights) return;
    
    // Move the last light to this position to avoid gaps
    if (lightIndex < lightingSystem->activeDynamicLights - 1) {
        lightingSystem->dynamicLights[lightIndex] = lightingSystem->dynamicLights[lightingSystem->activeDynamicLights - 1];
    }
    
    lightingSystem->activeDynamicLights--;
    lightingSystem->dynamicLights[lightingSystem->activeDynamicLights].active = false;
}

void UpdateDynamicLightPosition(LightingSystem* lightingSystem, uint32 lightIndex, vec3 newPosition) {
    if (!lightingSystem || lightIndex >= lightingSystem->activeDynamicLights) return;
    
    lightingSystem->dynamicLights[lightIndex].position = newPosition;
}

DynamicLight* GetDynamicLight(LightingSystem* lightingSystem, uint32 lightIndex) {
    if (!lightingSystem || lightIndex >= lightingSystem->activeDynamicLights) return nullptr;
    
    return &lightingSystem->dynamicLights[lightIndex];
}