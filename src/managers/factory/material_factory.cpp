//
// Created by Adam Socki on 6/5/25.
//

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

    material.descriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        AllocateMaterialDescriptorSet(zaynMem, &material, i);
    }

    uint32_t materialIndex = PushBack(&zaynMem->materialFactory.materials, material);
    Material* pointerToStoredMaterial = &zaynMem->materialFactory.materials[materialIndex];

    zaynMem->materialFactory.materialNamePointerMap[material.name] = pointerToStoredMaterial;
    zaynMem->materialFactory.availableMaterialNames.push_back(material.name);
    return pointerToStoredMaterial;
}

void InitMaterialFactory(Zayn* zaynMem)
{
    zaynMem->materialFactory.materials = MakeDynamicArray<Material>(&zaynMem->permanentMemory, 100);
}
