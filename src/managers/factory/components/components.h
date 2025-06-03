//
// Created by Adam Socki on 6/2/25.
//

enum ComponentsType {
    TransformComponents_Type,
    MeshComponents_Type,
    MaterialComponents_Type,

    ComponentsType_Count,
};

struct Components {
    EntityHandle owner;
};

