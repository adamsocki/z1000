//
// Created by Adam Socki on 6/5/25.
//

enum MaterialType
{
    MATERIAL_PBR,
    MATERIAL_UNLIT,
    MATERIAL_LIGHTING  // For basic lighting system following LearnOpenGL Colors tutorial
};



struct MaterialCreateInfo {
    MaterialType type = MATERIAL_PBR;
    Texture* texture;
    std::string name;
    float color[4];
    float roughness;
    float metallic;
};

struct Material {
    std::string name;
    MaterialType type;
    std::vector<VkDescriptorSet> descriptorSets;
    Texture* texture;
    // float color[4];
    //  float metallic;
    // float roughness;
    
    // For lighting materials - object color following LearnOpenGL Colors tutorial
    vec3 objectColor = V3(1.0f, 1.0f, 1.0f);  // Default white
    
    // Per-material lighting uniform buffers (only used for MATERIAL_LIGHTING)
    std::vector<VkBuffer> lightingUniformBuffers;
    std::vector<VkDeviceMemory> lightingUniformBuffersMemory;
    std::vector<void*> lightingUniformBuffersMapped;
    
    bool isInitialized = false;
};

struct MaterialFactory
{

    DynamicArray<Material> materials;

    std::unordered_map<std::string, Material*> materialNamePointerMap;
    std::vector<std::string> availableMaterialNames;
};

