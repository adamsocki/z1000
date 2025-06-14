#include <optional>
#include <vector>
#include <array>
#include <vulkan/vulkan.h>
#include <glm/glm.hpp>

#if IMGUI
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"

void InitMyImgui(Renderer* renderer, WindowManager* window);
void UpdateMyImgui(Zayn* zaynMem, LevelEditor* editor, Camera* camera, Renderer* renderer, WindowManager* windowManager, InputManager* inputManager);
#endif

void InitRender_Vulkan(Renderer* renderer, WindowManager* window)
{
    std::cout << "InitRender_Vulkan()" << std::endl;
    std::cout << "InitRender_Vulkan() with renderer address: " << renderer << std::endl;

    StartRender_Init(renderer, window);

#if IMGUI
    InitMyImgui(renderer, window);
#endif
    std::cout << "after InitRender_Vulkan()" << std::endl;

    CreateDescriptorSetLayout(renderer, &renderer->data.vkDescriptorSetLayout, true);
    CreateDescriptorPool(renderer, &renderer->data.vkDescriptorPool, true);

    CreatePushConstant<ModelPushConstant>(renderer);

    CreateGraphicsPipeline(renderer, &renderer->data.vkGraphicsPipeline, GetShaderPath("vkShader_3d_vert.spv"), GetShaderPath("vkShader_3d_frag.spv"), renderer->data.vkPushConstantRanges, &renderer->data.vkDescriptorSetLayout, &renderer->data.vkPipelineLayout);

    CreateUniformBuffer(renderer, renderer->data.vkUniformBuffers, renderer->data.vkUniformBuffersMemory, renderer->data.vkUniformBuffersMapped);

    CreateCommandBuffers(renderer);
    CreateSyncObjects(renderer);

    std::cout << "after InitRender_Vulkan() with vkRenderPass: " << renderer->data.vkRenderPass << std::endl;
}

VkResult AcquireNextImage(Renderer* renderer, uint32_t* imageIndex)
{
    vkWaitForFences(renderer->data.vkDevice, 1, &renderer->data.vkInFlightFences[renderer->data.vkCurrentFrame], VK_TRUE, UINT64_MAX);

    VkResult result = vkAcquireNextImageKHR(
            renderer->data.vkDevice,
            renderer->data.vkSwapChain,
            UINT64_MAX,
            renderer->data.vkImageAvailableSemaphores[renderer->data.vkCurrentFrame],
            VK_NULL_HANDLE,
            imageIndex);

    return result;
}

void CleanUpSwapChain(Renderer* renderer)
{
    vkDestroyImageView(renderer->data.vkDevice, renderer->data.vkDepthImageView, nullptr);
    vkDestroyImage(renderer->data.vkDevice, renderer->data.vkDepthImage, nullptr);
    vkFreeMemory(renderer->data.vkDevice, renderer->data.vkDepthImageMemory, nullptr);

    for (size_t i = 0; i < renderer->data.vkSwapChainFramebuffers.size(); i++)
    {
        vkDestroyFramebuffer(renderer->data.vkDevice, renderer->data.vkSwapChainFramebuffers[i], nullptr);
    }

    for (size_t i = 0; i < renderer->data.vkSwapChainImageViews.size(); i++)
    {
        vkDestroyImageView(renderer->data.vkDevice, renderer->data.vkSwapChainImageViews[i], nullptr);
    }

    vkDestroySwapchainKHR(renderer->data.vkDevice, renderer->data.vkSwapChain, nullptr);
}

void RecreateSwapChain(Renderer* renderer, WindowManager* windowManager)
{
    int width = 0, height = 0;
    glfwGetFramebufferSize(windowManager->glfwWindow, &width, &height);
    while (width == 0 || height == 0)
    {
        glfwGetFramebufferSize(windowManager->glfwWindow, &width, &height);
        glfwWaitEvents();
    }

    vkDeviceWaitIdle(renderer->data.vkDevice);

    CleanUpSwapChain(renderer);
    CreateSwapChain(renderer, windowManager);
    CreateImageViews(renderer);
    CreateDepthResources(renderer);
    CreateFrameBuffers(renderer);
}

bool BeginFrameRender(Renderer* renderer, WindowManager* windowManager)
{
    assert(!renderer->data.vkIsFrameStarted && "cannot call begin frame when frame buffer is already in progress");
    auto result = AcquireNextImage(renderer, &renderer->data.vkCurrentImageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        RecreateSwapChain(renderer, windowManager);
        return false;
    }
    else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
    {
        throw std::runtime_error("failed to acquire swap chain image!");
    }

    renderer->data.vkIsFrameStarted = true;

    vkResetFences(renderer->data.vkDevice, 1, &renderer->data.vkInFlightFences[renderer->data.vkCurrentFrame]);
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    if (vkBeginCommandBuffer(renderer->data.vkCommandBuffers[renderer->data.vkCurrentFrame], &beginInfo) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to begin recording command buffer!");
        return false;
    }

    return true;
}

void UpdateUniformBuffer(uint32_t currentImage, Renderer* renderer, Camera* cam)
{
    UniformBufferObject ubo = {};

    glm::vec3 camPos = glm::vec3(cam->pos.x, cam->pos.y, cam->pos.z);
    glm::vec3 camFront = glm::vec3(cam->front.x, cam->front.y, cam->front.z);
    glm::vec3 camUp = glm::vec3(cam->up.x, cam->up.y, cam->up.z);

    ubo.view = glm::lookAt(camPos, camPos + camFront, camUp);

    ubo.proj = glm::perspective(glm::radians(60.0f), renderer->data.vkSwapChainExtent.width / (float)renderer->data.vkSwapChainExtent.height, 0.1f, 1000.0f);
    ubo.proj[1][1] *= -1;

    memcpy(renderer->data.vkUniformBuffersMapped[currentImage], &ubo, sizeof(ubo));
}

void BeginSwapChainRenderPass(Renderer* renderer, VkCommandBuffer commandBuffer)
{
    assert(renderer->data.vkIsFrameStarted && "Can't call beginSwapChainRenderPass if frame is not in progress");
    assert(commandBuffer == renderer->data.vkCommandBuffers[renderer->data.vkCurrentFrame] && "Can't begin render pass on command buffer from a different frame");

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = renderer->data.vkRenderPass;
    renderPassInfo.framebuffer = renderer->data.vkSwapChainFramebuffers[renderer->data.vkCurrentImageIndex];
    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.renderArea.extent = renderer->data.vkSwapChainExtent;

    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = { {0.0f, 0.0f, 0.0f, 1.0f} };
    clearValues[1].depthStencil = { 1.0f, 0 };
    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(renderer->data.vkSwapChainExtent.width);
    viewport.height = static_cast<float>(renderer->data.vkSwapChainExtent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    VkRect2D scissor{ {0, 0}, renderer->data.vkSwapChainExtent };
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
}

void UpdateInstanceBuffer(Mesh* mesh) {
    if (!mesh->instanceDataRequiresGpuUpdate || mesh->instanceCount == 0) return;

    std::cout << "Updating instance buffer with " << mesh->instanceCount << " instances" << std::endl;

    for (uint32 i = 0; i < mesh->instanceCount; i++) {
        ((InstancedData*)mesh->instanceBufferMapped)[i] = mesh->instanceData[i];
    }

    mesh->instanceDataRequiresGpuUpdate = false;
}

void RenderMeshInstanced(VkCommandBuffer commandBuffer, Mesh* mesh, VkDescriptorSet descriptorSet, VkPipelineLayout pipelineLayout) {
    if (mesh->instanceCount == 0) return;

    UpdateInstanceBuffer(mesh);

    VkBuffer vertexBuffers[] = { mesh->vertexBuffer, mesh->instanceBuffer };
    VkDeviceSize offsets[] = { 0, 0 };
    vkCmdBindVertexBuffers(commandBuffer, 0, 2, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(commandBuffer, mesh->indexBuffer, 0, VK_INDEX_TYPE_UINT32);

    vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(mesh->indices.size()), mesh->instanceCount, 0, 0, 0);
}

void ClearMeshInstances(Mesh* mesh) {
    mesh->instanceCount = 0;
    mesh->instanceData.count = 0;
    mesh->registeredEntities.count = 0;
    mesh->instanceDataRequiresGpuUpdate = true;
}

void AddMeshInstance(Mesh* mesh, EntityHandle entityHandle, mat4 modelMatrix) {
    if (mesh->instanceCount >= mesh->maxInstances) {
        std::cout << "ERROR: Mesh instance limit reached!" << std::endl;
        return;
    }

    InstancedData instanceData = {};
    instanceData.modelMatrix = modelMatrix;

    PushBack(&mesh->instanceData, instanceData);
    PushBack(&mesh->registeredEntities, entityHandle);
    mesh->instanceCount++;
    mesh->instanceDataRequiresGpuUpdate = true;

    std::cout << "Added mesh instance. Total instances: " << mesh->instanceCount << std::endl;
}

void RenderInstancedMeshes(Zayn* zaynMem, VkCommandBuffer commandBuffer) {
    for (int i = 0; i < zaynMem->meshFactory.meshes.count; i++) {
        Mesh* mesh = &zaynMem->meshFactory.meshes[i];

        if (mesh->instanceCount == 0) continue;

        EntityHandle firstEntity = mesh->registeredEntities[0];
        uint32_t frameIndex = zaynMem->renderer.data.vkCurrentFrame % MAX_FRAMES_IN_FLIGHT;

        Material* material = nullptr;
        if (firstEntity.type == EntityType_Wall) {
            WallEntity* wallEntity = (WallEntity*)GetEntity(&zaynMem->entityFactory, firstEntity);
            material = wallEntity->material;
        }
        
        if (!material) continue;
        VkDescriptorSet& set = material->descriptorSets[frameIndex];

        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, zaynMem->renderer.data.vkGraphicsPipeline);
        vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, zaynMem->renderer.data.vkPipelineLayout, 0, 1, &set, 0, nullptr);

        RenderMeshInstanced(commandBuffer, mesh, set, zaynMem->renderer.data.vkPipelineLayout);
    }
}

void RenderEntities(Zayn* zaynMem, VkCommandBuffer commandBuffer) {
    EntityFactory* entityFactory = &zaynMem->entityFactory;
    ComponentsFactory * componentfactory = &zaynMem->componentsFactory;

    RenderInstancedMeshes(zaynMem, commandBuffer);

    for (int i = 0; i < entityFactory->activeEntityHandles.count; i++) {
        EntityHandle* entityHandle = &entityFactory->activeEntityHandles[i];
    }
}

void EndSwapChainRenderPass(Renderer* renderer, VkCommandBuffer commandBuffer)
{
    assert(renderer->data.vkIsFrameStarted && "Can't call beginSwapChainRenderPass if frame is not in progress");
    assert(commandBuffer == renderer->data.vkCommandBuffers[renderer->data.vkCurrentFrame] && "Can't begin render pass on command buffer from a different frame");

    vkCmdEndRenderPass(commandBuffer);
}

VkResult SubmitCommandBuffers(Renderer* renderer, std::vector<VkCommandBuffer> buffers, uint32_t* imageIndex)
{

    if (renderer->data.vkImagesInFlight[*imageIndex] != VK_NULL_HANDLE)
    {
        vkWaitForFences(renderer->data.vkDevice, 1, &renderer->data.vkImagesInFlight[*imageIndex], VK_TRUE, UINT64_MAX);
    }
    renderer->data.vkImagesInFlight[*imageIndex] = renderer->data.vkImagesInFlight[renderer->data.vkCurrentFrame];
    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = { renderer->data.vkImageAvailableSemaphores[renderer->data.vkCurrentFrame] };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = static_cast<uint32_t>(buffers.size());
    submitInfo.pCommandBuffers = buffers.data();

    VkSemaphore signalSemaphores[] = { renderer->data.vkRenderFinishedSemaphores[*imageIndex] };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    vkResetFences(renderer->data.vkDevice, 1, &renderer->data.vkInFlightFences[renderer->data.vkCurrentFrame]);
    if (vkQueueSubmit(renderer->data.vkGraphicsQueue, 1, &submitInfo, renderer->data.vkInFlightFences[renderer->data.vkCurrentFrame]) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to submit draw command buffer!");
    }

    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = { renderer->data.vkSwapChain };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;

    presentInfo.pImageIndices = imageIndex;

    auto result = vkQueuePresentKHR(renderer->data.vkPresentQueue, &presentInfo);

    renderer->data.vkCurrentFrame = (renderer->data.vkCurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;

    return result;
}

void EndFrameRender(Renderer* renderer, WindowManager* windowManager)
{
    assert(renderer->data.vkIsFrameStarted && "Can't call endFrame while frame is not in progress");
    std::vector<VkCommandBuffer> submitCommandBuffers = {};
    submitCommandBuffers.push_back(renderer->data.vkCommandBuffers[renderer->data.vkCurrentFrame]);

#if IMGUI
    if (renderer->myImgui.visible) {
        submitCommandBuffers.push_back(renderer->myImgui.imGuiCommandBuffers[renderer->data.vkCurrentFrame]);
    }
#endif

    if (vkEndCommandBuffer(submitCommandBuffers[0]) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to record command buffer!");
    }

    auto result = SubmitCommandBuffers(renderer, submitCommandBuffers, &renderer->data.vkCurrentImageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || renderer->data.vkFramebufferResized)
    {
        renderer->data.vkFramebufferResized = false;
        RecreateSwapChain(renderer, windowManager);
    }
    else if (result != VK_SUCCESS)
    {
        throw std::runtime_error("failed to present swap chain image!");
    }
    renderer->data.vkIsFrameStarted = false;
}

void UpdateRenderer(Zayn* zaynMem, Renderer* renderer, WindowManager* windowManager, Camera* camera, InputManager* inputManager)
{
    if (BeginFrameRender(renderer, windowManager))
    {

        UpdateUniformBuffer(renderer->data.vkCurrentFrame, renderer, camera);
        BeginSwapChainRenderPass(renderer, renderer->data.vkCommandBuffers[renderer->data.vkCurrentFrame]);

        if (false) {
        }
        else {
            RenderEntities(zaynMem, renderer->data.vkCommandBuffers[renderer->data.vkCurrentFrame]);
        }

#if IMGUI
        UpdateMyImgui(zaynMem, &zaynMem->levelEditor, camera, renderer, windowManager, inputManager);
#endif

    }
    EndSwapChainRenderPass(renderer, renderer->data.vkCommandBuffers[renderer->data.vkCurrentFrame]);
    EndFrameRender(renderer, windowManager);
}