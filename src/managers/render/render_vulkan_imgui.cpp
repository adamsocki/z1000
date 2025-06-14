// #include "render_vulkan_functions.h"
//
// #if IMGUI
//
// void InitMyImgui(Renderer* renderer, WindowManager* window) {
//     renderer->myImgui.visible = false;
//
//     IMGUI_CHECKVERSION();
//     ImGui::CreateContext();
//     ImGuiIO& io = ImGui::GetIO(); (void)io;
//     io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
//
//     ImGui::StyleColorsDark();
//
//     ImGui_ImplGlfw_InitForVulkan(window->glfwWindow, true);
//
//     ImGui_ImplVulkan_InitInfo init_info = {};
//     init_info.Instance = renderer->data.vkInstance;
//     init_info.PhysicalDevice = renderer->data.vkPhysicalDevice;
//     init_info.Device = renderer->data.vkDevice;
//     init_info.QueueFamily = 0;
//     init_info.Queue = renderer->data.vkGraphicsQueue;
//     init_info.PipelineCache = VK_NULL_HANDLE;
//     init_info.DescriptorPool = renderer->data.vkDescriptorPool;
//     init_info.DescriptorPoolSize = 1000;
//     init_info.Subpass = 0;
//     init_info.MinImageCount = 2;
//     init_info.ImageCount = static_cast<uint32_t>(renderer->data.vkSwapChainImages.size());
//     init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
//     init_info.Allocator = nullptr;
//     init_info.CheckVkResultFn = nullptr;
//     init_info.RenderPass = renderer->myImgui.imGuiRenderPass;
//
//     ImGui_ImplVulkan_Init(&init_info);
//
//     VkCommandBuffer command_buffer = BeginSingleTimeCommands(renderer);
//     ImGui_ImplVulkan_CreateFontsTexture();
//     EndSingleTimeCommands(renderer, command_buffer);
//
//     ImGui_ImplVulkan_DestroyFontsTexture();
// }
//
// void UpdateMyImgui(Zayn* zaynMem, LevelEditor* editor, Camera* camera, Renderer* renderer, WindowManager* windowManager, InputManager* inputManager) {
//     if (!editor->isActive) {
//         renderer->myImgui.visible = false;
//         return;
//     }
//
//     ImGui_ImplVulkan_NewFrame();
//     ImGui_ImplGlfw_NewFrame();
//     ImGui::NewFrame();
//
//     bool showEntityEditor = true;
//     if (ImGui::Begin("Entity Editor", &showEntityEditor)) {
//         if (editor->selectedEntity.isValid) {
//             EntityHandle handle = editor->selectedEntity.handle;
//
//             ImGui::Text("Selected Entity: %s (ID: %d)",
//                        editor->selectedEntity.type == EntityType_Wall ? "Wall" : "Unknown",
//                        handle.indexInInfo);
//
//             if (editor->selectedEntity.type == EntityType_Wall) {
//                 WallEntity* wall = (WallEntity*)GetEntity(&zaynMem->entityFactory, handle);
//                 if (wall) {
//                     ImGui::Separator();
//                     ImGui::Text("Transform");
//
//                     bool changed = false;
//                     changed |= ImGui::DragFloat3("Position", &wall->position.x, 0.1f);
//                     changed |= ImGui::DragFloat3("Rotation", &wall->rotation.x, 1.0f);
//                     changed |= ImGui::DragFloat3("Scale", &wall->scale.x, 0.1f, 0.1f, 10.0f);
//
//                     if (changed && wall->mesh) {
//                         for (uint32 i = 0; i < wall->mesh->instanceCount; i++) {
//                             EntityHandle instanceHandle = wall->mesh->registeredEntities[i];
//                             if (instanceHandle.indexInInfo == handle.indexInInfo &&
//                                 instanceHandle.generation == handle.generation) {
//
//                                 mat4 newTransform = TRS(wall->position, wall->rotation, wall->scale);
//                                 wall->mesh->instanceData[i].modelMatrix = newTransform;
//                                 wall->mesh->instanceDataRequiresGpuUpdate = true;
//                                 break;
//                             }
//                         }
//                     }
//                 }
//             }
//         } else {
//             ImGui::Text("No entity selected");
//             ImGui::Text("Press Space to cycle through entities");
//         }
//
//         ImGui::Separator();
//         ImGui::Text("Level Editor Controls:");
//         ImGui::Text("Tab - Toggle Editor Mode");
//         ImGui::Text("Space - Select Next Entity");
//         ImGui::Text("Backspace - Delete Selected Entity");
//         ImGui::Text("WASD/QE - Move Selected Entity");
//         ImGui::Text("Arrow Keys - Rotate Selected Entity");
//     }
//     ImGui::End();
//
//     bool showLevelTools = true;
//     if (ImGui::Begin("Level Tools", &showLevelTools)) {
//         ImGui::Text("Level Management");
//
//         if (ImGui::Button("Save Level")) {
//             ImGui::OpenPopup("Save Complete");
//         }
//
//         if (ImGui::BeginPopupModal("Save Complete", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
//             ImGui::Text("Level saved successfully!");
//             if (ImGui::Button("OK")) {
//                 ImGui::CloseCurrentPopup();
//             }
//             ImGui::EndPopup();
//         }
//
//         ImGui::SameLine();
//         if (ImGui::Button("Load Level")) {
//         }
//
//         ImGui::Separator();
//         ImGui::Text("Entity Creation");
//
//         if (ImGui::Button("Create Wall")) {
//             vec3 spawnPos = camera->position + camera->front * 5.0f;
//         }
//
//         ImGui::Separator();
//         ImGui::Text("Statistics");
//         ImGui::Text("Walls: %d", zaynMem->gameData.walls.count);
//     }
//     ImGui::End();
//
//     ImGui::Render();
//
//     VkCommandBuffer cmd = renderer->myImgui.imGuiCommandBuffers[renderer->data.vkCurrentFrame];
//
//     VkCommandBufferBeginInfo beginInfo = {};
//     beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
//     beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
//
//     vkBeginCommandBuffer(cmd, &beginInfo);
//
//     VkRenderPassBeginInfo renderPassInfo = {};
//     renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
//     renderPassInfo.renderPass = renderer->myImgui.imGuiRenderPass;
//     renderPassInfo.framebuffer = renderer->myImgui.imGuiFrameBuffers[renderer->data.vkCurrentImageIndex];
//     renderPassInfo.renderArea.offset = {0, 0};
//     renderPassInfo.renderArea.extent = renderer->data.vkSwapChainExtent;
//
//     vkCmdBeginRenderPass(cmd, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
//
//     ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd);
//
//     vkCmdEndRenderPass(cmd);
//     vkEndCommandBuffer(cmd);
//
//     renderer->myImgui.visible = true;
// }
// #endif