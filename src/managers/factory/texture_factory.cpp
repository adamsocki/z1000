//
// Created by Adam Socki on 6/6/25.
//

void CreateTextureImageView(Renderer* renderer, uint32_t& mipLevels, VkImage* textureImage, VkImageView* textureImageView)
{
    *textureImageView = CreateImageView(*textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, mipLevels, renderer);
}

void CreateTextureImage(Renderer* renderer, uint32_t& mipLevels, VkImage* textureImage, VkDeviceMemory* textureImageMemory, const std::string texturePath, VkFormat format)
{
    int texWidth, texHeight, texChannels;
    stbi_uc* pixels = stbi_load(texturePath.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    VkDeviceSize imageSize = texWidth * texHeight * 4;
    mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(texWidth, texHeight)))) + 1;

    if (!pixels)
    {
        throw std::runtime_error("failed to load texture image!");
    }

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;

    CreateBuffer(renderer, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);
    void* data;
    vkMapMemory(renderer->data.vkDevice, stagingBufferMemory, 0, imageSize, 0, &data);
    memcpy(data, pixels, static_cast<size_t>(imageSize));
    vkUnmapMemory(renderer->data.vkDevice, stagingBufferMemory);

    stbi_image_free(pixels);

    CreateImage(texWidth, texHeight, mipLevels, format, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, *textureImage, *textureImageMemory, renderer); // added

    // TransitionImageLayout(zaynMem->vkTextureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, zaynMem);
    TransitionImageLayout(renderer, *textureImage, format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, mipLevels);
    CopyBufferToImage(renderer, stagingBuffer, *textureImage, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));

    vkDestroyBuffer(renderer->data.vkDevice, stagingBuffer, nullptr);
    vkFreeMemory(renderer->data.vkDevice, stagingBufferMemory, nullptr);
    GenerateMipmaps(renderer, *textureImage, format, texWidth, texHeight, mipLevels);
}

void CreateTextureSampler(Renderer* rederer, uint32_t& mipLevels, VkSampler* textureSampler)
{
    VkPhysicalDeviceProperties properties = {};
    vkGetPhysicalDeviceProperties(rederer->data.vkPhysicalDevice, &properties);

    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.anisotropyEnable = VK_TRUE;
    samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod = static_cast<float>(mipLevels);
    samplerInfo.mipLodBias = 0.0f;

    if (vkCreateSampler(rederer->data.vkDevice, &samplerInfo, nullptr, textureSampler) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create texture sampler!");
    }
}

Texture* MakeTexture(Zayn* zaynMem, TextureCreateInfo* info)
{
    Texture texture = {};
    Renderer* renderer = &zaynMem->renderer;
    texture.name = info->name;

    CreateTextureImage(renderer, texture.mipLevels, &texture.image, &texture.memory, GetTexturePath(info->path), info->format);
    CreateTextureImageView(renderer, texture.mipLevels, &texture.image, &texture.view);
    CreateTextureSampler(renderer, texture.mipLevels, &texture.sampler);

    uint32_t materialIndex = PushBack(&zaynMem->textureFactory.textures, texture);
    Texture* pointerToStoredTexture = &zaynMem->textureFactory.textures[materialIndex];

    zaynMem->textureFactory.textureNamePointerMap[texture.name] = pointerToStoredTexture;
    zaynMem->textureFactory.availableTextureNames.push_back(texture.name);
    return pointerToStoredTexture;
}
void InitTextureFactory(Zayn* zaynMem)
{
    zaynMem->textureFactory.textures = MakeDynamicArray<Texture>(&zaynMem->permanentMemory, 100);
}