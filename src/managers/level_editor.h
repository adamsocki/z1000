//
// Created by Level Editor
//

enum EditorMode {
    EditorMode_Select,
    EditorMode_Move,
    EditorMode_Rotate
};

struct SelectedEntity {
    EntityHandle handle;
    EntityType type;
    bool isValid;
};

struct LevelEditor {
    bool isActive;
    EditorMode currentMode;
    
    SelectedEntity selectedEntity;
    
    // Movement speeds
    real32 moveSpeed;
    real32 rotateSpeed;
    
    // UI state for level management
    int selectedLevelIndex;
    bool requestLoadLevel;
    bool requestSaveLevel;
    bool requestCreateLevel;
    char newLevelName[64];
    char selectedLevelFile[128];
};