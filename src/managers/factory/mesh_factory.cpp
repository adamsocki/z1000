//
// Created by Adam Socki on 6/4/25.
//

#include <stdio.h>
#include <stdlib.h>

#define STB_IMAGE_IMPLEMENTATION
#include "../../include/stb_image.h"

struct MeshCreationInfo
{
    std::string path;
    std::string name;

};

std::string GetTexturePath(const std::string& filename) {
    std::string relativePath = "../src/render/textures/" + filename;

    std::ifstream testFile(relativePath);
    if (testFile.good()) {
        testFile.close();
        return relativePath;
    }

#ifdef WIN32
    // Try Windows-specific paths
    std::string winPath = std::string(PROJECT_DIR_PC) + "src/render/models/" + filename;
    testFile.open(winPath);
    if (testFile.good()) {
        testFile.close();
        return winPath;
    }
#elif __APPLE__
    // Try Mac-specific paths
    std::string macPath = std::string(PROJECT_DIR_MAC) + "src/managers/render/textures/" + filename;      // project directoryx1
    testFile.open(macPath);
    if (testFile.good()) {
        testFile.close();
        return macPath;
    }
#endif

    printf("Warning: Unable to find model file: %s, falling back to relative path\n", filename.c_str());
    return relativePath;
}

std::string GetModelPath(const std::string& filename) {
    std::string relativePath = "../src/render/models/" + filename;

    std::ifstream testFile(relativePath);
    if (testFile.good()) {
        testFile.close();
        return relativePath;
    }

#ifdef WIN32
    // Try Windows-specific paths
    std::string winPath = std::string(PROJECT_DIR_PC) + "src/render/models/" + filename;
    testFile.open(winPath);
    if (testFile.good()) {
        testFile.close();
        return winPath;
    }
#elif __APPLE__
    // Try Mac-specific paths
    std::string macPath = std::string(PROJECT_DIR_MAC) + "src/managers/render/models/" + filename;      // project directoryx1
    testFile.open(macPath);
    if (testFile.good()) {
        testFile.close();
        return macPath;
    }
#endif

    printf("Warning: Unable to find model file: %s, falling back to relative path\n", filename.c_str());
    return relativePath;
}

void CreateIndexBuffer(Renderer* renderer, std::vector<uint32_t> indices, VkBuffer* indexBuffer, VkDeviceMemory* indexBufferMemory)
{
    if (indices.empty())
    {
        return;
    }
    VkDeviceSize bufferSize = sizeof(uint32_t) * indices.size();

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    CreateBuffer(renderer, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

    void* data;
    vkMapMemory(renderer->data.vkDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, indices.data(), (size_t)bufferSize);
    vkUnmapMemory(renderer->data.vkDevice, stagingBufferMemory);

    CreateBuffer(renderer, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, *indexBuffer, *indexBufferMemory);

    CopyBuffer(renderer, stagingBuffer, *indexBuffer, bufferSize);

    vkDestroyBuffer(renderer->data.vkDevice, stagingBuffer, nullptr);
    vkFreeMemory(renderer->data.vkDevice, stagingBufferMemory, nullptr);
}

void CreateVertexBuffer(Renderer* renderer, std::vector<Vertex>& vertices, VkBuffer* vertexBuffer, VkDeviceMemory* vertexBufferMemory)
{
    if (vertices.empty())
    {
        return;
    }

    VkDeviceSize bufferSize = sizeof(Vertex) * vertices.size();
    // STAGING BUFFER - CPU accessible memory to upload the data from the vertex array to.
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    CreateBuffer(renderer, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

    void* data;
    vkMapMemory(renderer->data.vkDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, vertices.data(), (size_t)bufferSize);
    vkUnmapMemory(renderer->data.vkDevice, stagingBufferMemory);

    CreateBuffer(renderer, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, *vertexBuffer, *vertexBufferMemory);

    CopyBuffer(renderer,stagingBuffer, *vertexBuffer, bufferSize);

    vkDestroyBuffer(renderer->data.vkDevice, stagingBuffer, nullptr);
    vkFreeMemory(renderer->data.vkDevice, stagingBufferMemory, nullptr);
}

void LoadModel(std::string modelPath, std::vector<Vertex>* vertices, std::vector<uint32_t>* indices) {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, modelPath.c_str()))
    {
        throw std::runtime_error(warn + err);
    }
    std::unordered_map<Vertex, uint32_t> uniqueVertices{};

    for (const auto& shape : shapes)
    {
        for (const auto& index : shape.mesh.indices)
        {
            Vertex vertex{};

            // Load the vertex position directly (without coordinate system change)
            vertex.pos = {
                attrib.vertices[3 * index.vertex_index + 0],
                attrib.vertices[3 * index.vertex_index + 1],
                attrib.vertices[3 * index.vertex_index + 2] };

            vertex.texCoord = {
                attrib.texcoords[2 * index.texcoord_index + 0],
                1.0f - attrib.texcoords[2 * index.texcoord_index + 1] };

            vertex.color = { 0.3f, 1.0f, 0.6f };

            // Set default normal (pointing up in Y direction)
            vertex.normal = { 0.0f, 1.0f, 0.0f };

            if (uniqueVertices.count(vertex) == 0)
            {
                uniqueVertices[vertex] = static_cast<uint32_t>(vertices->size());
                vertices->push_back(vertex);
            }

            indices->push_back(uniqueVertices[vertex]);
        }
    }
    std::cout << vertices->size() << "<- vertices " << uniqueVertices.size() << "\n";

}

Mesh* MakeMesh(Zayn* zaynMem, MeshCreationInfo* info) {
    Renderer* renderer = &zaynMem->renderer;
    Mesh mesh ={};
    mesh.path = info->path;
    mesh.name = info->name;
    LoadModel(mesh.path, &mesh.vertices, &mesh.indices);
    CreateVertexBuffer(renderer, mesh.vertices, &mesh.vertexBuffer, &mesh.vertexBufferMemory);
    CreateIndexBuffer(renderer, mesh.indices, &mesh.indexBuffer, &mesh.indexBufferMemory);

    uint32_t meshIndex = PushBack(&zaynMem->meshFactory.meshes, mesh);
    Mesh* pointerToStoredMesh = &zaynMem->meshFactory.meshes[meshIndex];
    zaynMem->meshFactory.meshNamePointerMap[mesh.name] = pointerToStoredMesh;
    zaynMem->meshFactory.availableMeshNames.push_back(mesh.name);

    return pointerToStoredMesh;
}


void InitMeshFactory(MeshFactory* meshFactory, MemoryArena* arena) {
    meshFactory->meshes = MakeDynamicArray<Mesh>(arena, 100);
}