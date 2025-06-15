# Entity Creation Guide

This guide documents the step-by-step process for adding new entity types to the Z1000 game engine.

## Overview

The Z1000 engine uses a type-based entity system with sequential includes (no function declarations in headers). Each entity type requires updates to multiple files to be properly integrated.

## Step-by-Step Process

### 1. Create Entity Header and Implementation Files

**Location**: `src/game/entities/`

Create two files:
- `YourEntityName.h` - Entity struct definition
- `YourEntityName.cpp` - Entity implementation (can be empty initially)

**Example**: For LightSourceEntity
```cpp
// LightSourceEntity.h
struct LightSourceEntity: Entity {
    vec3 position = V3(0, 0, 0);
    vec3 color = V3(1, 1, 1);
    
    // Optional visual representation
    Mesh* mesh = nullptr;
    Material* material = nullptr;
};

// LightSourceEntity.cpp
// Implementation code or comments
```

### 2. Update Entity Factory Enum

**File**: `src/managers/factory/entity_factory.h`

Add your new entity type to the `EntityType` enum:
```cpp
enum EntityType 
{
    EntityType_Player,
    EntityType_Floor,
    EntityType_Piano,
    EntityType_Wall,
    EntityType_YourNewEntity,  // Add here
    EntityType_Count           // Must be last
};
```

### 3. Register Entity Type in Factory

**File**: `src/managers/factory/entity_factory.cpp`

Add your entity to the `InitEntityTypeInfo` function:
```cpp
void InitEntityTypeInfo(Zayn* zaynMem) {
    constexpr EntityTypeInfoForBuffer defaultEntityTypeInfo[] = {
        [EntityType_Player] = {EntityType_Player, sizeof(int32), 10, "Player"},
        [EntityType_Floor]  = {EntityType_Floor, sizeof(int32), 10000, "Floor"},
        [EntityType_Piano]  = {EntityType_Piano, sizeof(int32), 10, "Grand Piano"},
        [EntityType_Wall]  = {EntityType_Wall, sizeof(WallEntity), 100, "Wall"},
        [EntityType_YourNewEntity] = {EntityType_YourNewEntity, sizeof(YourEntityStruct), 50, "Display Name"},
        // Add more here...
    };
    // ...
}
```

**Parameters**:
- `EntityType_YourNewEntity`: The enum value
- `sizeof(YourEntityStruct)`: Size of your entity struct
- `50`: Default capacity (how many can be created)
- `"Display Name"`: Human-readable name for UI

### 4. Add to Game Data Structure

**File**: `src/game/game.h`

Include your entity header and add a DynamicArray to track instances:
```cpp
#include "entities/WallEntity.h"
#include "entities/YourNewEntity.h"  // Add include

struct GameData {
    DynamicArray<EntityHandle> walls;
    DynamicArray<EntityHandle> yourNewEntities;  // Add array
};
```

**File**: `src/game/game.cpp`

Initialize the array in `InitEntityHandleBuffers`:
```cpp
void InitEntityHandleBuffers(GameData* gameData, MemoryArena* arena) {
    gameData->walls = MakeDynamicArray<EntityHandle>(arena, 10);
    gameData->yourNewEntities = MakeDynamicArray<EntityHandle>(arena, 10);  // Add this
}
```

### 5. Add Sequential Include

**File**: `src/game/game.cpp`

Add your entity implementation to the includes:
```cpp
#include "entities/WallEntity.cpp"
#include "entities/YourNewEntity.cpp"  // Add this line
```

### 6. Update Level Editor Creation System

**File**: `src/managers/level_editor.cpp`

Add creation logic to `CreateEntityAtPosition` function:
```cpp
EntityHandle CreateEntityAtPosition(Zayn* zaynMem, EntityType entityType, vec3 position, int meshIndex, int materialIndex) {
    switch (entityType) {
        case EntityType_Wall: {
            // Existing wall creation code...
            return handle;
        }
        
        case EntityType_YourNewEntity: {
            EntityHandle handle = AddEntity(&zaynMem->entityFactory, EntityType_YourNewEntity);
            YourEntityStruct* entity = (YourEntityStruct*)GetEntity(&zaynMem->entityFactory, handle);
            
            if (entity) {
                entity->position = position;
                // Set other properties...
                
                // Assign mesh and material if needed
                if (meshIndex >= 0 && meshIndex < zaynMem->meshFactory.meshes.count) {
                    entity->mesh = &zaynMem->meshFactory.meshes[meshIndex];
                }
                
                // Add to renderer if visual
                if (entity->mesh) {
                    mat4 transform = TRS(position, V3(0,0,0), V3(1,1,1));
                    AddMeshInstance(entity->mesh, handle, transform);
                }
                
                // Add to game data
                PushBack(&zaynMem->gameData.yourNewEntities, handle);
            }
            
            return handle;
        }
        
        default:
            return {};
    }
}
```

### 7. Update Level Editor UI

**File**: `src/managers/render/render_vulkan_core.cpp`

Add your entity type to the dropdown in `UpdateMyImgui`:
```cpp
// Find this line:
const char* entityTypeNames[] = {"Player", "Floor", "Piano", "Wall"};

// Update to:
const char* entityTypeNames[] = {"Player", "Floor", "Piano", "Wall", "Your Entity"};
```

### 8. Update Entity Selection System

**File**: `src/managers/level_editor.cpp`

Add handling for your entity in `UpdateSelectedEntityTransform` and `DeleteSelectedEntity`:
```cpp
void UpdateSelectedEntityTransform(Zayn* zaynMem, LevelEditor* editor) {
    if (!editor->selectedEntity.isValid) return;
    
    EntityHandle handle = editor->selectedEntity.handle;
    
    if (editor->selectedEntity.type == EntityType_Wall) {
        // Existing wall code...
    }
    else if (editor->selectedEntity.type == EntityType_YourNewEntity) {
        YourEntityStruct* entity = (YourEntityStruct*)GetEntity(&zaynMem->entityFactory, handle);
        if (!entity) return;
        
        // Add transform update logic...
    }
}
```

### 9. Update Level Manager (Serialization)

**File**: `src/managers/level_manager.cpp`

Add string conversion in `StringToEntityType`:
```cpp
EntityType StringToEntityType(const char* typeName) {
    if (strcmp(typeName, "Wall") == 0) return EntityType_Wall;
    if (strcmp(typeName, "YourEntity") == 0) return EntityType_YourNewEntity;  // Add this
    // ... other types
    return EntityType_Player; // Default fallback
}
```

Add loading logic in `LoadLevel` function:
```cpp
// In the entity loading loop, add:
else if (entityType == EntityType_YourNewEntity) {
    YourEntityStruct* entity = (YourEntityStruct*)GetEntity(&zaynMem->entityFactory, handle);
    if (entity) {
        // Load properties from JSON...
        // Add to game data
        PushBack(&zaynMem->gameData.yourNewEntities, handle);
    }
}
```

### 10. Update Statistics Display

**File**: `src/managers/render/render_vulkan_core.cpp`

Add to the statistics section in ImGui:
```cpp
ImGui::Separator();
ImGui::Text("Statistics");
ImGui::Text("Walls: %d", zaynMem->gameData.walls.count);
ImGui::Text("Your Entities: %d", zaynMem->gameData.yourNewEntities.count);  // Add this
```

## Quick Checklist

When adding a new entity type, ensure you've updated:

- [ ] Created `YourEntity.h` and `YourEntity.cpp` files
- [ ] Added to `EntityType` enum in `entity_factory.h`
- [ ] Registered in `InitEntityTypeInfo` in `entity_factory.cpp`
- [ ] Added include to `game.h`
- [ ] Added DynamicArray to `GameData` struct in `game.h`
- [ ] Initialized array in `InitEntityHandleBuffers` in `game.cpp`
- [ ] Added sequential include in `game.cpp`
- [ ] Added creation logic to `CreateEntityAtPosition` in `level_editor.cpp`
- [ ] Updated entity type names array in `render_vulkan_core.cpp`
- [ ] Added transform handling in `UpdateSelectedEntityTransform`
- [ ] Added deletion handling in `DeleteSelectedEntity`
- [ ] Added string conversion in `StringToEntityType`
- [ ] Added loading logic in `LoadLevel`
- [ ] Updated statistics display

## Common Gotchas

1. **Include Order**: Always maintain sequential includes - headers don't declare functions
2. **EntityType_Count**: Must always be the last enum value
3. **Memory Size**: Use `sizeof(YourActualStruct)` not `sizeof(int32)` for real entities
4. **Array Initialization**: Don't forget to initialize the DynamicArray in `InitEntityHandleBuffers`
5. **Sequential Includes**: Add `.cpp` includes in `game.cpp`, not individual files
6. **Game Data**: Remember to add entities to the appropriate game data array
7. **Renderer Integration**: Call `AddMeshInstance` if your entity should be rendered

## Entity Categories

### Visual Entities (have mesh/material)
- Walls, Props, Decorations
- Need mesh assignment and renderer registration
- Should be added to appropriate visual entity array

### Logic Entities (invisible game logic)
- Triggers, Waypoints, Spawn Points
- No mesh/material needed
- May need special handling in editor for visualization

### Light Entities
- Light sources for lighting system
- May have optional visual representation
- Need integration with lighting uniforms/shaders

## Special Considerations for Lighting Systems

When adding lighting-related entities, additional steps are required:

### 11. Create Separate Lighting Shaders

**Location**: `src/managers/render/shaders/`

Create shader pairs for each lighting iteration:
```
vkShader_lighting_basic.vert/frag    - Basic color multiplication
vkShader_lighting_ambient.vert/frag  - Add ambient lighting
vkShader_lighting_diffuse.vert/frag  - Add diffuse lighting
```

This maintains separation between original and lighting-specific rendering.

### 12. Add Lighting Uniform Buffers

**File**: `src/managers/render/render_vulkan.h`

Create separate uniform buffer structs:
```cpp
struct LightingUniformBuffer {
    alignas(16) glm::vec3 lightColor;
    alignas(16) glm::vec3 objectColor;
};
```

### 13. Update Vulkan Pipeline for Lighting

Add new descriptor set layouts and pipelines for lighting shaders:
- Create new descriptor set layout with lighting uniform binding
- Compile lighting shaders to .spv format
- Create separate graphics pipeline for lit objects
- Manage lighting uniform buffer updates

### 14. Compile New Shaders

**Location**: `src/managers/render/shaders/`

Run the compile script to generate .spv files:
```bash
cd src/managers/render/shaders
./compile.sh
```

The script automatically compiles all .vert and .frag files, including new lighting shaders.

### 15. Add Lighting Uniform Buffer Updates

**File**: `src/managers/render/render_vulkan_core.cpp`

Create a lighting uniform buffer update function:
```cpp
void UpdateLightingUniformBuffer(uint32_t currentImage, Renderer* renderer, Zayn* zaynMem) {
    LightingUniformBuffer lightingUbo = {};
    
    // Set lighting values from light entities
    lightingUbo.lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
    lightingUbo.objectColor = glm::vec3(1.0f, 0.5f, 0.31f);
    
    // Use first light source if available
    if (zaynMem->gameData.lightSources.count > 0) {
        EntityHandle lightHandle = zaynMem->gameData.lightSources[0];
        LightSourceEntity* light = (LightSourceEntity*)GetEntity(&zaynMem->entityFactory, lightHandle);
        if (light) {
            lightingUbo.lightColor = glm::vec3(light->color.x, light->color.y, light->color.z);
        }
    }
    
    memcpy(renderer->data.vkLightingUniformBuffersMapped[currentImage], &lightingUbo, sizeof(lightingUbo));
}
```

Call this function in the `UpdateRenderer` loop:
```cpp
UpdateUniformBuffer(renderer->data.vkCurrentFrame, renderer, camera);
UpdateLightingUniformBuffer(renderer->data.vkCurrentFrame, renderer, zaynMem);
```

### 16. Initialize Lighting Pipeline

**File**: `src/managers/render/render_vulkan_init.cpp`

First, update `CreateDescriptorSetLayout` to support lighting uniforms:
```cpp
void CreateDescriptorSetLayout(Renderer* renderer, VkDescriptorSetLayout* descriptorSetLayout, bool hasImage, bool hasLighting = false)
{
    // ... existing bindings for 0 and 1 ...
    
    if (hasLighting)
    {
        VkDescriptorSetLayoutBinding lightingLayoutBinding{};
        lightingLayoutBinding.binding = 2;
        lightingLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        lightingLayoutBinding.descriptorCount = 1;
        lightingLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
        bindings.push_back(lightingLayoutBinding);
    }
}
```

Add lighting pipeline initialization in `InitRender_Vulkan`:
```cpp
// Create lighting pipeline following LearnOpenGL Colors tutorial
CreateDescriptorSetLayout(renderer, &renderer->data.vkLightingDescriptorSetLayout, true, true);
CreateDescriptorPool(renderer, &renderer->data.vkLightingDescriptorPool, true);
CreateGraphicsPipeline(renderer, &renderer->data.vkLightingGraphicsPipeline, 
    GetShaderPath("vkShader_lighting_basic_vert.spv"), 
    GetShaderPath("vkShader_lighting_basic_frag.spv"), 
    renderer->data.vkPushConstantRanges, 
    &renderer->data.vkLightingDescriptorSetLayout, 
    &renderer->data.vkLightingPipelineLayout);

// Create lighting uniform buffers
CreateUniformBuffer(renderer, renderer->data.vkLightingUniformBuffers, 
    renderer->data.vkLightingUniformBuffersMemory, 
    renderer->data.vkLightingUniformBuffersMapped);
```

Add `VkDescriptorPool vkLightingDescriptorPool;` to the Data struct in render_vulkan.h.

## Lighting Implementation Checklist

For lighting-specific entities, additional verification:

- [ ] Created separate lighting shaders (not modifying originals)
- [ ] Added LightingUniformBuffer struct to render_vulkan.h
- [ ] Updated Vulkan descriptor sets for lighting uniforms
- [ ] Compiled lighting shaders to .spv format
- [ ] Created separate graphics pipeline for lighting
- [ ] Added lighting uniform buffer management in render loop
- [ ] Integrated light entity data with uniform buffer updates
- [ ] Added toggle between lit/unlit rendering for testing

This guide ensures consistent entity integration following the engine's C-style, sequential include architecture.