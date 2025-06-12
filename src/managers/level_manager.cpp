//
// Created by Level Manager
//

#include <nlohmann/json.hpp>
#include <fstream>
#include <cstring>

using json = nlohmann::json;


std::string GetLevelPath(const std::string& filename) {
    std::string relativePath = "../levels/" + filename;

    std::ifstream testFile(relativePath);
    if (testFile.good()) {
        testFile.close();
        return relativePath;
    }

#ifdef WIN32
    // Try Windows-specific paths
    std::string winPath = std::string(PROJECT_DIR_PC) + "src/render/models/" + filename;
    testFile.open(winPath);
    if (testFile.good()) {
        testFile.close();
        return winPath;
    }
#elif __APPLE__
    // Try Mac-specific paths
    std::string macPath = std::string(PROJECT_DIR_MAC) + "levels/" + filename;      // project directoryx1
    testFile.open(macPath);
    if (testFile.good()) {
        testFile.close();
        return macPath;
    }
#endif

    printf("Warning: Unable to find model file: %s, falling back to relative path\n", filename.c_str());
    return relativePath;
}



void InitLevelManager(LevelManager* levelManager) {
    if (!levelManager) return;
    
    memset(&levelManager->currentLevel, 0, sizeof(LevelData));
    strcpy(levelManager->currentLevel.levelName, "Untitled");
    strcpy(levelManager->currentLevel.version, "1.0");
    levelManager->currentLevel.entityCount = 0;
    levelManager->isLevelLoaded = false;
}

EntityType StringToEntityType(const char* typeName) {
    if (strcmp(typeName, "Wall") == 0) return EntityType_Wall;
    if (strcmp(typeName, "Player") == 0) return EntityType_Player;
    if (strcmp(typeName, "Floor") == 0) return EntityType_Floor;
    if (strcmp(typeName, "Piano") == 0) return EntityType_Piano;
    return EntityType_Player; // Default fallback
}

void ClearLevel(Zayn* zaynMem) {
    if (!zaynMem) return;

    // Clear mesh instances from renderer
    for (uint32 i = 0; i < zaynMem->meshFactory.meshes.count; i++) {
        Mesh* mesh = &zaynMem->meshFactory.meshes[i];
        ClearMeshInstances(mesh);
    }
    
    // Clear walls from game data
    zaynMem->gameData.walls.count = 0;

    // Reset entity factory counts (simple approach)
    for (int i = 0; i < EntityType_Count; i++) {
        zaynMem->entityFactory.buffers[i].count = 0;
    }
  
    // Reset level data
    zaynMem->levelManager.currentLevel.entityCount = 0;
    strcpy(zaynMem->levelManager.currentLevel.levelName, "Untitled");
    strcpy(zaynMem->levelManager.currentLevel.version, "1.0");
}

bool LoadLevel(Zayn* zaynMem, const char* fileName) {
    if (!zaynMem || !fileName) return false;
    
    std::ifstream file(GetLevelPath(fileName));
    if (!file.is_open()) {
        printf("ERROR: Failed to open level file: %s\n", fileName);
        return false;
    }
    
    json levelJson;
    try {
        file >> levelJson;
    } catch (const json::exception& e) {
        printf("ERROR: Failed to parse JSON: %s\n", e.what());
        return false;
    }
    
    // Clear current level
    ClearLevel(zaynMem);
    
    // Load level metadata
    if (levelJson.contains("levelName")) {
        strncpy(zaynMem->levelManager.currentLevel.levelName, 
                levelJson["levelName"].get<std::string>().c_str(), 63);
    }
    if (levelJson.contains("version")) {
        strncpy(zaynMem->levelManager.currentLevel.version, 
                levelJson["version"].get<std::string>().c_str(), 15);
    }
    
    // Load entities
    if (levelJson.contains("entities") && levelJson["entities"].is_array()) {
        for (const auto& entityJson : levelJson["entities"]) {
            if (zaynMem->levelManager.currentLevel.entityCount >= MAX_LEVEL_ENTITIES) {
                printf("WARNING: Max entities reached, skipping remaining entities\n");
                break;
            }
            
            std::string typeStr = entityJson.value("type", "Wall");
            EntityType entityType = StringToEntityType(typeStr.c_str());
            
            // Create entity using factory
            EntityHandle handle = AddEntity(&zaynMem->entityFactory, entityType);
            
            // Get the created entity
            if (entityType == EntityType_Wall) {
                WallEntity* wall = (WallEntity*)GetEntity(&zaynMem->entityFactory, handle);
                if (wall) {
                    // Set transform
                    if (entityJson.contains("position") && entityJson["position"].is_array()) {
                        auto pos = entityJson["position"];
                        wall->position = V3(pos[0].get<float>(), pos[1].get<float>(), pos[2].get<float>());
                    }
                    if (entityJson.contains("rotation") && entityJson["rotation"].is_array()) {
                        auto rot = entityJson["rotation"];
                        wall->rotation = V3(rot[0].get<float>(), rot[1].get<float>(), rot[2].get<float>());
                    }
                    if (entityJson.contains("scale") && entityJson["scale"].is_array()) {
                        auto scale = entityJson["scale"];
                        wall->scale = V3(scale[0].get<float>(), scale[1].get<float>(), scale[2].get<float>());
                    }
                    
                    // Set material if specified
                    if (entityJson.contains("materialName")) {
                        std::string matName = entityJson["materialName"];
                        auto it = zaynMem->materialFactory.materialNamePointerMap.find(matName);
                        if (it != zaynMem->materialFactory.materialNamePointerMap.end()) {
                            wall->material = it->second;
                            printf("Assigned material: %s\n", matName.c_str());
                        } else {
                            printf("Material not found: %s\n", matName.c_str());
                        }
                    }
                    
                    // Set mesh if specified
                    if (entityJson.contains("meshName")) {
                        std::string meshName = entityJson["meshName"];
                        // For now, use a default mesh - you'd need to implement mesh lookup by name
                        // wall->mesh = FindMeshByName(zaynMem, meshName.c_str());
                    }
                    
                    // Use default mesh if no mesh assigned
                    if (!wall->mesh) {
                        // Try to find a default mesh from existing meshes
                        if (zaynMem->meshFactory.meshes.count > 0) {
                            wall->mesh = &zaynMem->meshFactory.meshes[0]; // Use first available mesh
                            printf("Assigned default mesh: %s (mesh count: %d)\n", wall->mesh->name.c_str(), zaynMem->meshFactory.meshes.count);
                        } else {
                            printf("ERROR: No meshes available in mesh factory!\n");
                        }
                    }
                    
                    // Use default material if no material assigned
                    if (!wall->material) {
                        if (zaynMem->materialFactory.materials.count > 0) {
                            wall->material = &zaynMem->materialFactory.materials[0];
                            printf("Assigned default material: %s\n", wall->material->name.c_str());
                        } else {
                            printf("ERROR: No materials available in material factory!\n");
                        }
                    }
                    
                    // Register with renderer - this is the missing piece!
                    if (wall->mesh && wall->material) {
                        mat4 transform = TRS(wall->position, wall->rotation, wall->scale);
                        AddMeshInstance(wall->mesh, handle, transform);
                        printf("Added mesh instance at position (%.1f, %.1f, %.1f)\n", 
                               wall->position.x, wall->position.y, wall->position.z);
                    } else {
                        printf("ERROR: Cannot render wall - missing mesh or material\n");
                    }
                    
                    // Store in game data
                    PushBack(&zaynMem->gameData.walls, handle);
                }
            }
            
            // Store entity info in level data
            LevelEntity& levelEntity = zaynMem->levelManager.currentLevel.entities[zaynMem->levelManager.currentLevel.entityCount];
            strncpy(levelEntity.typeName, typeStr.c_str(), 31);
            levelEntity.id = zaynMem->levelManager.currentLevel.entityCount;
            
            zaynMem->levelManager.currentLevel.entityCount++;
        }
    }
    
    zaynMem->levelManager.isLevelLoaded = true;
    printf("Level loaded: %s (%d entities)\n", 
           zaynMem->levelManager.currentLevel.levelName, 
           zaynMem->levelManager.currentLevel.entityCount);
    
    return true;
}

bool SaveLevel(Zayn* zaynMem, const char* fileName) {
    if (!zaynMem || !fileName) return false;
    
    json levelJson;
    
    // Save level metadata
    levelJson["levelName"] = zaynMem->levelManager.currentLevel.levelName;
    levelJson["version"] = zaynMem->levelManager.currentLevel.version;
    
    // Save entities
    levelJson["entities"] = json::array();
    
    // Save walls
    for (uint32 i = 0; i < zaynMem->gameData.walls.count; i++) {
        EntityHandle handle = zaynMem->gameData.walls[i];
        WallEntity* wall = (WallEntity*)GetEntity(&zaynMem->entityFactory, handle);
        
        if (wall && wall->isActive) {
            json entityJson;
            entityJson["type"] = "Wall";
            entityJson["id"] = i;
            
            // Transform
            entityJson["position"] = {wall->position.x, wall->position.y, wall->position.z};
            entityJson["rotation"] = {wall->rotation.x, wall->rotation.y, wall->rotation.z};
            entityJson["scale"] = {wall->scale.x, wall->scale.y, wall->scale.z};
            
            // Material
            if (wall->material) {
                entityJson["materialName"] = wall->material->name;
            }
            
            // Add to entities array
            levelJson["entities"].push_back(entityJson);
        }
    }
    
    // Write to file
    std::ofstream file(GetLevelPath(fileName));
    if (!file.is_open()) {
        printf("ERROR: Failed to create level file: %s\n", fileName);
        return false;
    }
    
    file << levelJson.dump(2); // Pretty print with 2 space indentation
    file.close();
    
    printf("Level saved: %s (%d entities)\n", fileName, (int)levelJson["entities"].size());
    return true;
}

