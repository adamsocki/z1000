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

};



struct MeshComponents: Components {
    Mesh mesh;
};


