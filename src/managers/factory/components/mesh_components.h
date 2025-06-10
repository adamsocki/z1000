//
// Created by Adam Socki on 6/2/25.
//

struct Mesh {

    std::string name;
    std::string path;
    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;
    VkBuffer indexBuffer;
    VkDeviceMemory indexBufferMemory;
    uint32_t indexCount;
    uint32_t vertexCount;

    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    bool isInitialized = false;


    // Instancing support
    bool supportsInstancing = false;
    VkBuffer instanceBuffer = VK_NULL_HANDLE;
    VkDeviceMemory instanceBufferMemory = VK_NULL_HANDLE;
    void* instanceBufferMapped = nullptr;
    uint32_t instanceCount = 0;
    uint32_t maxInstances = 0;

    DynamicArray<InstancedData> instanceData;
    DynamicArray<EntityHandle> registeredEntities;
    bool instanceDataRequiresGpuUpdate = true;

};



struct MeshComponents: Components {
    Mesh mesh;
};


