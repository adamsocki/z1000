//
// Created by Adam Socki on 6/5/25.
//

bool AllocateLightingMaterialDescriptorSet(Zayn* zaynMem, Material* material, uint32_t frameIndex) {
    Renderer* renderer = &zaynMem->renderer;
    
    // Create per-material uniform buffer if not already created
    if (material->lightingUniformBuffers[frameIndex] == VK_NULL_HANDLE) {
        CreateUniformBuffer(zaynMem, &material->lightingUniformBuffers[frameIndex], 
                          &material->lightingUniformBuffersMemory[frameIndex], 
                          &material->lightingUniformBuffersMapped[frameIndex],
                          sizeof(LightingUniformBuffer));
    }
    
    // For lighting materials, we need the lighting descriptor set layout
    VkDescriptorSetAllocateInfo allocInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };
    allocInfo.descriptorPool = renderer->data.vkLightingDescriptorPool;
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = &renderer->data.vkLightingDescriptorSetLayout;
    
    VkResult result = vkAllocateDescriptorSets(renderer->data.vkDevice, &allocInfo, &material->descriptorSets[frameIndex]);
    if (result != VK_SUCCESS) {
        fprintf(stderr, "Failed to allocate lighting material descriptor set: %d\n", result);
        return false;
    }
    
    // Set up descriptor bindings for lighting materials
    std::array<VkWriteDescriptorSet, 3> descriptorWrites{};
    
    // Binding 0: Camera uniform buffer
    VkDescriptorBufferInfo bufferInfo = {};
    bufferInfo.buffer = renderer->data.vkUniformBuffers[frameIndex];
    bufferInfo.offset = 0;
    bufferInfo.range = sizeof(UniformBufferObject);
    
    descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrites[0].dstSet = material->descriptorSets[frameIndex];
    descriptorWrites[0].dstBinding = 0;
    descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptorWrites[0].descriptorCount = 1;
    descriptorWrites[0].pBufferInfo = &bufferInfo;
    
    // Binding 1: Texture (if material has one)
    VkDescriptorImageInfo imageInfo{};
    if (material->texture && material->texture->view && material->texture->sampler) {
        imageInfo.sampler = material->texture->sampler;
        imageInfo.imageView = material->texture->view;
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        
        descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[1].dstSet = material->descriptorSets[frameIndex];
        descriptorWrites[1].dstBinding = 1;
        descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptorWrites[1].descriptorCount = 1;
        descriptorWrites[1].pImageInfo = &imageInfo;
    }
    
    // Binding 2: Lighting uniform buffer - use per-material buffer
    VkDescriptorBufferInfo lightingBufferInfo = {};
    lightingBufferInfo.buffer = material->lightingUniformBuffers[frameIndex];
    lightingBufferInfo.offset = 0;
    lightingBufferInfo.range = sizeof(LightingUniformBuffer);
    
    descriptorWrites[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrites[2].dstSet = material->descriptorSets[frameIndex];
    descriptorWrites[2].dstBinding = 2;
    descriptorWrites[2].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptorWrites[2].descriptorCount = 1;
    descriptorWrites[2].pBufferInfo = &lightingBufferInfo;
    
    // Update descriptor sets (skip texture binding if not present)
    uint32_t writeCount = material->texture ? 3 : 2;
    if (!material->texture) {
        // Shift lighting buffer to index 1 if no texture
        descriptorWrites[1] = descriptorWrites[2];
        writeCount = 2;
    }
    
    vkUpdateDescriptorSets(renderer->data.vkDevice, writeCount, descriptorWrites.data(), 0, nullptr);
    return true;
}

bool AllocateMaterialDescriptorSet(Zayn* zaynMem, Material* material, uint32_t frameIndex) {
        Renderer* renderer = &zaynMem->renderer;
        // Validate required components
        if (!material->texture || !material->texture->view || !material->texture->sampler) {
            fprintf(stderr, "Material missing valid albedo texture\n");
            return false;
        }

        VkDescriptorSetLayout layout = material->texture
                                       ? renderer->data.vkDescriptorSetLayout
                                       : renderer->data.vkDescriptorSetLayout_blank;

        VkDescriptorPool pool = material->texture
                                ? renderer->data.vkDescriptorPool
                                : renderer->data.vkDescriptorPool_blank;
        // Descriptor set allocation
        VkDescriptorSetAllocateInfo allocInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };

        allocInfo.descriptorPool = renderer->data.vkDescriptorPool;
        allocInfo.descriptorSetCount = 1;
        allocInfo.pSetLayouts = &renderer->data.vkDescriptorSetLayout;

        VkResult result = vkAllocateDescriptorSets(renderer->data.vkDevice, &allocInfo, &material->descriptorSets[frameIndex]);
        if (result != VK_SUCCESS) {
            fprintf(stderr, "Failed to allocate material descriptor set: %d\n", result);
            return false;
        }

        VkDescriptorBufferInfo bufferInfo = {};
        bufferInfo.buffer = renderer->data.vkUniformBuffers[frameIndex]; // Use frame's uniform buffer
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(UniformBufferObject);

        // Bind the material's texture to the descriptor set
        VkDescriptorImageInfo imageInfo{};
        imageInfo.sampler = material->texture->sampler;
        imageInfo.imageView = material->texture->view;
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

        std::array<VkWriteDescriptorSet, 2> descriptorWrites{};

        descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[0].dstSet = material->descriptorSets[frameIndex];
        descriptorWrites[0].dstBinding = 0;
        descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrites[0].descriptorCount = 1;
        descriptorWrites[0].pBufferInfo = &bufferInfo;

        descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[1].dstSet = material->descriptorSets[frameIndex];
        descriptorWrites[1].dstBinding = 1;
        descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptorWrites[1].descriptorCount = 1;
        descriptorWrites[1].pImageInfo = &imageInfo;

        vkUpdateDescriptorSets(renderer->data.vkDevice, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);


        return true;
    }

Material* MakeMaterial(Zayn* zaynMem, MaterialCreateInfo* info)
{
    Material material = {};
    material.type = info->type;
    //outMaterial->color = info->color;
    //memcpy(outMaterial->color, info->color, sizeof(float) * 4);
    //        material.roughness = info->roughness;
    //        material.metallic = info->metallic;
    material.texture = info->texture;
    material.name = info->name;
    //        material.color = info->color;
    
    // Set object color for lighting materials
    if (info->type == MATERIAL_LIGHTING) {
        material.objectColor = V3(info->color[0], info->color[1], info->color[2]);
        
        // Initialize vectors but don't create buffers yet - will be created lazily when needed
        material.lightingUniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
        material.lightingUniformBuffersMemory.resize(MAX_FRAMES_IN_FLIGHT);
        material.lightingUniformBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);
        
        // Initialize to null - will be created when first used
        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            material.lightingUniformBuffers[i] = VK_NULL_HANDLE;
            material.lightingUniformBuffersMemory[i] = VK_NULL_HANDLE;
            material.lightingUniformBuffersMapped[i] = nullptr;
        }
    }

    material.descriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        if (material.type == MATERIAL_LIGHTING) {
            AllocateLightingMaterialDescriptorSet(zaynMem, &material, i);
        } else {
            AllocateMaterialDescriptorSet(zaynMem, &material, i);
        }
    }

    uint32_t materialIndex = PushBack(&zaynMem->materialFactory.materials, material);
    Material* pointerToStoredMaterial = &zaynMem->materialFactory.materials[materialIndex];

    zaynMem->materialFactory.materialNamePointerMap[material.name] = pointerToStoredMaterial;
    zaynMem->materialFactory.availableMaterialNames.push_back(material.name);
    return pointerToStoredMaterial;
}

void CreateBasicLightingMaterials(Zayn* zaynMem) {
    // Create basic lighting materials with different colors following LearnOpenGL Colors tutorial
    
    // Coral (original tutorial color)
    MaterialCreateInfo coralInfo = {};
    coralInfo.type = MATERIAL_LIGHTING;
    coralInfo.texture = nullptr;  // No texture needed for basic lighting
    coralInfo.name = "Lighting - Coral";
    coralInfo.color[0] = 1.0f; coralInfo.color[1] = 0.5f; coralInfo.color[2] = 0.31f; coralInfo.color[3] = 1.0f;
    MakeMaterial(zaynMem, &coralInfo);
    
    // Red
    MaterialCreateInfo redInfo = {};
    redInfo.type = MATERIAL_LIGHTING;
    redInfo.texture = nullptr;
    redInfo.name = "Lighting - Red";
    redInfo.color[0] = 1.0f; redInfo.color[1] = 0.0f; redInfo.color[2] = 0.0f; redInfo.color[3] = 1.0f;
    MakeMaterial(zaynMem, &redInfo);
    
    // Green
    MaterialCreateInfo greenInfo = {};
    greenInfo.type = MATERIAL_LIGHTING;
    greenInfo.texture = nullptr;
    greenInfo.name = "Lighting - Green";
    greenInfo.color[0] = 0.0f; greenInfo.color[1] = 1.0f; greenInfo.color[2] = 0.0f; greenInfo.color[3] = 1.0f;
    MakeMaterial(zaynMem, &greenInfo);
    
    // Blue
    MaterialCreateInfo blueInfo = {};
    blueInfo.type = MATERIAL_LIGHTING;
    blueInfo.texture = nullptr;
    blueInfo.name = "Lighting - Blue";
    blueInfo.color[0] = 0.0f; blueInfo.color[1] = 0.0f; blueInfo.color[2] = 1.0f; blueInfo.color[3] = 1.0f;
    MakeMaterial(zaynMem, &blueInfo);
    
    // Yellow
    MaterialCreateInfo yellowInfo = {};
    yellowInfo.type = MATERIAL_LIGHTING;
    yellowInfo.texture = nullptr;
    yellowInfo.name = "Lighting - Yellow";
    yellowInfo.color[0] = 1.0f; yellowInfo.color[1] = 1.0f; yellowInfo.color[2] = 0.0f; yellowInfo.color[3] = 1.0f;
    MakeMaterial(zaynMem, &yellowInfo);
    
    // Purple
    MaterialCreateInfo purpleInfo = {};
    purpleInfo.type = MATERIAL_LIGHTING;
    purpleInfo.texture = nullptr;
    purpleInfo.name = "Lighting - Purple";
    purpleInfo.color[0] = 1.0f; purpleInfo.color[1] = 0.0f; purpleInfo.color[2] = 1.0f; purpleInfo.color[3] = 1.0f;
    MakeMaterial(zaynMem, &purpleInfo);
    
    // White
    MaterialCreateInfo whiteInfo = {};
    whiteInfo.type = MATERIAL_LIGHTING;
    whiteInfo.texture = nullptr;
    whiteInfo.name = "Lighting - White";
    whiteInfo.color[0] = 1.0f; whiteInfo.color[1] = 1.0f; whiteInfo.color[2] = 1.0f; whiteInfo.color[3] = 1.0f;
    MakeMaterial(zaynMem, &whiteInfo);
}

void InitMaterialFactory(Zayn* zaynMem)
{
    zaynMem->materialFactory.materials = MakeDynamicArray<Material>(&zaynMem->permanentMemory, 100);
}
