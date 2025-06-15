//
// Created by Adam Socki on 6/4/25.
//

#ifndef MESH_FACTORY_H
#define MESH_FACTORY_H

#include <vector>
#include <unordered_map>
#include <string>
#include <vulkan/vulkan.h>
#include "../../utils/dynamic_array.h"

// Forward declaration to avoid circular dependency
struct InstancedData;

struct Mesh {
    std::vector<InstancedData> instances;
    VkBuffer instanceBuffer;
    VkDeviceMemory instanceBufferMemory;
    uint32_t instanceCount;
    
    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;
    VkBuffer indexBuffer;
    VkDeviceMemory indexBufferMemory;
    uint32_t indexCount;
};

struct MeshFactory {
    DynamicArray<Mesh> meshes;
    std::unordered_map<std::string, Mesh*> meshNamePointerMap;

    std::vector<std::string> availableMeshNames;
};

#endif // MESH_FACTORY_H

