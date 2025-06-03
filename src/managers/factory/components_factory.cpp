//
// Created by Adam Socki on 6/1/25.
//


#include "components/transform_components.cpp"
#include "components/mesh_components.cpp"
#include "components/material_components.cpp"


void InitComponentsFactory(ComponentsFactory* factory, MemoryArena* arena) {
    factory->componentsStorage.transformComponents =        MakeDynamicArray<TransformComponents>(arena, 1280);
    factory->componentsStorage.meshComponents =             MakeDynamicArray<MeshComponents>(arena, 1280);
    factory->componentsStorage.materialComponents =         MakeDynamicArray<MaterialComponents>(arena, 1280);

}




void UpdateComponentsFactory(Components* components) {

}