//
// Created by Adam Socki on 6/4/25.
//



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

