//
// Created by Level Editor
//

void InitLevelEditor(LevelEditor* editor) {
    if (!editor) return;
    
    editor->isActive = false;
    editor->currentMode = EditorMode_Select;
    editor->selectedEntity.isValid = false;
    editor->moveSpeed = 0.1f;
    editor->rotateSpeed = 5.0f;
    
    // Initialize UI state
    editor->selectedLevelIndex = 0;
    editor->requestLoadLevel = false;
    editor->requestSaveLevel = false;
    editor->requestCreateLevel = false;
    strcpy(editor->newLevelName, "new_level");
    strcpy(editor->selectedLevelFile, "");
    
    // Initialize entity creation UI state
    editor->selectedEntityTypeForCreation = EntityType_Wall;
    editor->selectedMeshForCreation = 0;
    editor->selectedMaterialForCreation = 0;
    editor->requestCreateEntity = false;
    editor->entitySpawnPosition = V3(0, 0, 0);
    
    // Initialize light creation settings
    editor->lightColorForCreation = V3(1, 1, 1);  // Default white light
}

void SelectEntity(LevelEditor* editor, EntityHandle handle, EntityType type) {
    editor->selectedEntity.handle = handle;
    editor->selectedEntity.type = type;
    editor->selectedEntity.isValid = true;
    
    printf("Selected entity: type=%d, index=%d\n", type, handle.indexInInfo);
}

void DeselectEntity(LevelEditor* editor) {
    editor->selectedEntity.isValid = false;
    printf("Deselected entity\n");
}

void UpdateSelectedEntityTransform(Zayn* zaynMem, LevelEditor* editor) {
    if (!editor->selectedEntity.isValid) {
        return;
    }
    
    EntityHandle handle = editor->selectedEntity.handle;
    
    // Only handle walls for now
    if (editor->selectedEntity.type == EntityType_Wall) {
        WallEntity* wall = (WallEntity*)GetEntity(&zaynMem->entityFactory, handle);
        if (!wall) return;
        
        vec3 positionDelta = V3(0, 0, 0);
        vec3 rotationDelta = V3(0, 0, 0);
        bool needsUpdate = false;
        
        // Movement controls (WASD + QE for up/down)
        if (InputHeld(zaynMem->inputManager.keyboard, Input_W)) {
            positionDelta.z += editor->moveSpeed;
            needsUpdate = true;
        }
        if (InputHeld(zaynMem->inputManager.keyboard, Input_S)) {
            positionDelta.z -= editor->moveSpeed;
            needsUpdate = true;
        }
        if (InputHeld(zaynMem->inputManager.keyboard, Input_A)) {
            positionDelta.x += editor->moveSpeed;
            needsUpdate = true;
        }
        if (InputHeld(zaynMem->inputManager.keyboard, Input_D)) {
            positionDelta.x -= editor->moveSpeed;
            needsUpdate = true;
        }
        if (InputHeld(zaynMem->inputManager.keyboard, Input_Q)) {
            positionDelta.y -= editor->moveSpeed;
            needsUpdate = true;
        }
        if (InputHeld(zaynMem->inputManager.keyboard, Input_E)) {
            positionDelta.y += editor->moveSpeed;
            needsUpdate = true;
        }
        
        // Rotation controls (Arrow keys)
        if (InputHeld(zaynMem->inputManager.keyboard, Input_LeftArrow)) {
            rotationDelta.y -= editor->rotateSpeed;
            needsUpdate = true;
        }
        if (InputHeld(zaynMem->inputManager.keyboard, Input_RightArrow)) {
            rotationDelta.y += editor->rotateSpeed;
            needsUpdate = true;
        }
        if (InputHeld(zaynMem->inputManager.keyboard, Input_UpArrow)) {
            rotationDelta.x -= editor->rotateSpeed;
            needsUpdate = true;
        }
        if (InputHeld(zaynMem->inputManager.keyboard, Input_DownArrow)) {
            rotationDelta.x += editor->rotateSpeed;
            needsUpdate = true;
        }
        
        // Update transform if changed
        if (needsUpdate) {
            wall->position = wall->position + positionDelta;
            wall->rotation = wall->rotation + rotationDelta;
            
            // Update the mesh instance transform
            if (wall->mesh) {
                std::cout << "wall update" << std::endl;
                std::cout << positionDelta.x << ", " << positionDelta.y << ", " << positionDelta.z << std::endl;
                // Find and update the mesh instance
                for (uint32 i = 0; i < wall->mesh->instanceCount; i++) {
                    EntityHandle instanceHandle = wall->mesh->registeredEntities[i];
                    if (instanceHandle.indexInInfo == handle.indexInInfo && 
                        instanceHandle.generation == handle.generation) {
                        
                        mat4 newTransform = TRS(wall->position, wall->rotation, wall->scale);
                        wall->mesh->instanceData[i].modelMatrix = newTransform;
                        wall->mesh->instanceDataRequiresGpuUpdate = true;
                        break;
                    }
                }
            }
        }
    }
}

void DeleteSelectedEntity(Zayn* zaynMem, LevelEditor* editor) {
    if (!editor->selectedEntity.isValid) return;
    
    EntityHandle handle = editor->selectedEntity.handle;
    
    if (editor->selectedEntity.type == EntityType_Wall) {
        WallEntity* wall = (WallEntity*)GetEntity(&zaynMem->entityFactory, handle);
        if (!wall) return;
        
        // Remove from mesh instances
        if (wall->mesh) {
            for (uint32 i = 0; i < wall->mesh->instanceCount; i++) {
                EntityHandle instanceHandle = wall->mesh->registeredEntities[i];
                if (instanceHandle.indexInInfo == handle.indexInInfo && 
                    instanceHandle.generation == handle.generation) {
                    
                    // Remove instance by swapping with last
                    if (i < wall->mesh->instanceCount - 1) {
                        wall->mesh->instanceData[i] = wall->mesh->instanceData[wall->mesh->instanceCount - 1];
                        wall->mesh->registeredEntities[i] = wall->mesh->registeredEntities[wall->mesh->instanceCount - 1];
                    }
                    wall->mesh->instanceCount--;
                    wall->mesh->instanceDataRequiresGpuUpdate = true;
                    break;
                }
            }
        }
        
        // Remove from walls array
        for (uint32 i = 0; i < zaynMem->gameData.walls.count; i++) {
            EntityHandle wallHandle = zaynMem->gameData.walls[i];
            if (wallHandle.indexInInfo == handle.indexInInfo && 
                wallHandle.generation == handle.generation) {
                
                // Remove by swapping with last
                if (i < zaynMem->gameData.walls.count - 1) {
                    zaynMem->gameData.walls[i] = zaynMem->gameData.walls[zaynMem->gameData.walls.count - 1];
                }
                zaynMem->gameData.walls.count--;
                break;
            }
        }
        
        // Mark entity as inactive
        wall->isActive = false;
        
        printf("Deleted wall entity\n");
    }
    
    DeselectEntity(editor);
}

EntityHandle CreateWallEntity(Zayn* zaynMem, vec3 position, vec3 rotation, vec3 scale) {
    EntityHandle handle = AddEntity(&zaynMem->entityFactory, EntityType_Wall);
    WallEntity* wall = (WallEntity*)GetEntity(&zaynMem->entityFactory, handle);
    
    if (wall) {
        wall->position = position;
        wall->rotation = rotation;
        wall->scale = scale;
        wall->isActive = true;
        
        // Assign default mesh and material if available
        if (zaynMem->meshFactory.meshes.count > 0) {
            wall->mesh = &zaynMem->meshFactory.meshes[0];
        }
        if (zaynMem->materialFactory.materials.count > 0) {
            wall->material = &zaynMem->materialFactory.materials[0];
        }
        
        // Add to renderer
        if (wall->mesh && wall->material) {
            mat4 transform = TRS(position, rotation, scale);
            AddMeshInstance(wall->mesh, handle, transform);
        }
        
        // Add to game data
        PushBack(&zaynMem->gameData.walls, handle);
    }
    
    return handle;
}

EntityHandle CreateEntityAtPosition(Zayn* zaynMem, EntityType entityType, vec3 position, int meshIndex, int materialIndex) {
    switch (entityType) {
        case EntityType_Wall: {
            EntityHandle handle = AddEntity(&zaynMem->entityFactory, EntityType_Wall);
            WallEntity* wall = (WallEntity*)GetEntity(&zaynMem->entityFactory, handle);
            
            if (wall) {
                wall->position = position;
                wall->rotation = V3(0, 0, 0);
                wall->scale = V3(1, 1, 1);
                wall->isActive = true;
                
                // Assign mesh
                if (meshIndex >= 0 && meshIndex < zaynMem->meshFactory.meshes.count) {
                    wall->mesh = &zaynMem->meshFactory.meshes[meshIndex];
                } else if (zaynMem->meshFactory.meshes.count > 0) {
                    wall->mesh = &zaynMem->meshFactory.meshes[0];
                }
                
                // Assign material
                if (materialIndex >= 0 && materialIndex < zaynMem->materialFactory.materials.count) {
                    wall->material = &zaynMem->materialFactory.materials[materialIndex];
                } else if (zaynMem->materialFactory.materials.count > 0) {
                    wall->material = &zaynMem->materialFactory.materials[0];
                }
                
                // Add to renderer
                if (wall->mesh && wall->material) {
                    mat4 transform = TRS(position, wall->rotation, wall->scale);
                    AddMeshInstance(wall->mesh, handle, transform);
                }
                
                // Add to game data
                PushBack(&zaynMem->gameData.walls, handle);
            }
            
            return handle;
        }
        
        case EntityType_LightSource: {
            EntityHandle handle = AddEntity(&zaynMem->entityFactory, EntityType_LightSource);
            LightSourceEntity* light = (LightSourceEntity*)GetEntity(&zaynMem->entityFactory, handle);
            
            if (light) {
                light->position = position;
                light->color = zaynMem->levelEditor.lightColorForCreation;  // Use color from UI
                light->isActive = true;
                
                // Optionally assign a small mesh for visual representation in editor
                if (meshIndex >= 0 && meshIndex < zaynMem->meshFactory.meshes.count) {
                    light->mesh = &zaynMem->meshFactory.meshes[meshIndex];
                }
                
                // Assign material - light sources should NOT use lighting materials
                // Find first non-lighting material for light source visual representation
                light->material = nullptr;
                for (uint32 i = 0; i < zaynMem->materialFactory.materials.count; i++) {
                    Material* mat = &zaynMem->materialFactory.materials[i];
                    if (mat->type != MATERIAL_LIGHTING) {
                        light->material = mat;
                        break;
                    }
                }
                
                // If user specifically selected a material and it's not lighting, use it
                if (materialIndex >= 0 && materialIndex < zaynMem->materialFactory.materials.count) {
                    Material* selectedMat = &zaynMem->materialFactory.materials[materialIndex];
                    if (selectedMat->type != MATERIAL_LIGHTING) {
                        light->material = selectedMat;
                    }
                }
                
                // Add to renderer for visual debugging if we have both mesh and material
                if (light->mesh && light->material) {
                    mat4 transform = TRS(position, V3(0,0,0), V3(0.2f, 0.2f, 0.2f)); // Small scale
                    AddMeshInstance(light->mesh, handle, transform);
                }
                
                // Add to game data
                PushBack(&zaynMem->gameData.lightSources, handle);
                
                printf("Created light source with color (%.1f, %.1f, %.1f)\n", 
                       light->color.x, light->color.y, light->color.z);
            }
            
            return handle;
        }
        
        // Add other entity types here as needed
        default:
            printf("Entity type %d not yet supported for creation\n", entityType);
            return {};
    }
}

void UpdateLevelEditor(Zayn* zaynMem, LevelEditor* editor) {
    // Toggle editor mode first - before the early return
    if (InputPressed(zaynMem->inputManager.keyboard, Input_Tab)) {
        editor->isActive = !editor->isActive;
        printf("Level editor %s\n", editor->isActive ? "enabled" : "disabled");
        return;
    }
    
    if (!editor->isActive) return;
    
    // Delete selected entity
    if (InputPressed(zaynMem->inputManager.keyboard, Input_Backspace)) {
        DeleteSelectedEntity(zaynMem, editor);
    }
    
    // Selection (for now, just cycle through walls with Space)
    if (InputPressed(zaynMem->inputManager.keyboard, Input_Space)) {

        if (zaynMem->gameData.walls.count > 0) {
            std::cout << zaynMem->gameData.walls.count << std::endl; 
            static uint32 currentSelection = 0;
            if (currentSelection >= zaynMem->gameData.walls.count) {
                currentSelection = 0;
            }
            
            EntityHandle handle = zaynMem->gameData.walls[currentSelection];
            SelectEntity(editor, handle, EntityType_Wall);
            currentSelection = (currentSelection + 1) % zaynMem->gameData.walls.count;
        }
    }
    
    // Update selected entity transform
    UpdateSelectedEntityTransform(zaynMem, editor);
    
    // Handle level management requests from UI
    if (editor->requestLoadLevel) {
        LoadLevel(zaynMem, editor->selectedLevelFile);
        editor->requestLoadLevel = false;
    }
    
    if (editor->requestSaveLevel) {
        SaveLevel(zaynMem, editor->selectedLevelFile);
        editor->requestSaveLevel = false;
    }
    
    if (editor->requestCreateLevel) {
        std::string fileName = std::string(editor->newLevelName) + ".json";
        ClearLevel(zaynMem);
        strcpy(zaynMem->levelManager.currentLevel.levelName, editor->newLevelName);
        SaveLevel(zaynMem, fileName.c_str());
        editor->requestCreateLevel = false;
    }
    
    // Handle entity creation request
    if (editor->requestCreateEntity) {
        EntityHandle newEntity = CreateEntityAtPosition(zaynMem, (EntityType)editor->selectedEntityTypeForCreation, 
                                                      editor->entitySpawnPosition, editor->selectedMeshForCreation, editor->selectedMaterialForCreation);
        
        if (newEntity.indexInInfo >= 0) {
            SelectEntity(editor, newEntity, (EntityType)editor->selectedEntityTypeForCreation);
            printf("Created entity of type %d at position (%.1f, %.1f, %.1f)\n", 
                   editor->selectedEntityTypeForCreation, editor->entitySpawnPosition.x, editor->entitySpawnPosition.y, editor->entitySpawnPosition.z);
        }
        
        editor->requestCreateEntity = false;
    }
}