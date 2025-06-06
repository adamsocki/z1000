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
inline bool AreHandlesEqual(EntityHandle h1, EntityHandle h2) {
    return h1.indexInInfo == h2.indexInInfo &&
           h1.generation == h2.generation &&
           h1.type == h2.type;
}

template<typename T>
    inline T* FindComponentsInArray(DynamicArray<T>* componentArray, EntityHandle entityHandle) {
    if (!componentArray) {
        return nullptr;
    }
    for (uint32 i = 0; i < componentArray->count; ++i) {
        // Compare the owner handle of the component in the array with the target handle
        if (AreHandlesEqual((*componentArray)[i].owner, entityHandle)) {
            // Found it, return pointer to the data in the array
            return &(*componentArray)[i];
        }
    }
    return nullptr; // Not found
}

struct ComponentsStorage {
    DynamicArray<TransformComponents>   transformComponents;
    DynamicArray<MeshComponents>        meshComponents;
    DynamicArray<MaterialComponents>    materialComponents;
};

struct ComponentsFactory {
    ComponentsStorage componentsStorage;
};


