//
// Created by Adam Socki on 6/2/25.
//

#include <vulkan/vulkan.h>
#include <cstring>

#define MAX_INSTANCES 1000

void CreateInstanceBuffer(Mesh* mesh, Data* renderer) {
    VkDeviceSize bufferSize = sizeof(InstancedData) * MAX_INSTANCES;
    
    CreateBuffer(renderer, bufferSize,
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        mesh->instanceBuffer, mesh->instanceBufferMemory);
}

void UpdateInstanceBuffer(Mesh* mesh, Data* renderer) {
    VkDeviceSize bufferSize = sizeof(InstancedData) * mesh->instances.size();
    void* data;
    vkMapMemory(renderer->vkDevice, mesh->instanceBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, mesh->instances.data(), bufferSize);
    vkUnmapMemory(renderer->vkDevice, mesh->instanceBufferMemory);
}

void InitMeshComponents() {

}
