//
// Created by Adam Socki on 6/5/25.
//

enum MaterialType
{
    MATERIAL_PBR,
    MATERIAL_UNLIT
};

struct Texture {

};

struct Material {
    std::string name;
    MaterialType type;
    std::vector<VkDescriptorSet> descriptorSets;
    Texture* texture;
    // float color[4];
    //  float metallic;
    // float roughness;
    bool isInitialized = false;
};


struct MaterialFactory
{
    DynamicArray<Material> materials;

    std::unordered_map<std::string, Material*> materialNamePointerMap;
    std::vector<std::string> availableMaterialNames;
};

