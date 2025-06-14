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
}