//
// Created by Adam Socki on 6/6/25.
//



struct TextureCreateInfo
{
    std::string path;
    std::string name;
    VkFormat format = VK_FORMAT_R8G8B8A8_SRGB;
    bool generateMipmaps;
    //VkFilter filter;
    VkSamplerAddressMode addressMode;
};

struct Texture {

    std::string name;
    VkImage image;
    VkDeviceMemory memory;
    VkImageView view;
    VkSampler sampler;
    uint32_t width;
    uint32_t height;
    uint32_t mipLevels = 1;

    bool isInitialized = false;
};





struct TextureFactory
{
    std::unordered_map<std::string, Texture*> textureNamePointerMap;
    std::vector<std::string> availableTextureNames;

    DynamicArray<Texture> textures;
};
