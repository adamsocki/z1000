//
// Created by Adam Socki on 6/1/25.
//

#include "components/components.h"

#include "components/transform_components.h"
#include "components/mesh_components.h"
#include "components/material_components.h"


template<typename T>
    inline T* AddComponents(DynamicArray<T>* componentArray, EntityHandle entityHandle) {
    if (!componentArray) {
        printf("Error: Component array pointer is null.\n");
        return nullptr;
    }

    T newComp = {};
    newComp.owner = entityHandle;
    uint32 index = PushBack(componentArray, newComp);


    return &(*componentArray)[index];
}

struct ComponentsStorage {
    DynamicArray<TransformComponents>   transformComponents;
    DynamicArray<MeshComponents>        meshComponents;
    DynamicArray<MaterialComponents>    materialComponents;
};

struct ComponentsFactory {
    ComponentsStorage componentsStorage;
};


