
#include <optional>
#include <vector>
#include <array>
#include <vulkan/vulkan.h>
#include <glm/glm.hpp>
#ifdef NDEBUG
const bool enableValidationLayers = true;
#else
const bool enableValidationLayers = true;
#endif

const std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };
// const std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };


#ifdef WINDOWS
#elif __APPLE__
const std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME, "VK_KHR_portability_subset" };
#endif






bool checkValidationLayerSupport()
{
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    VkLayerProperties* availableLayers = (VkLayerProperties*)malloc(layerCount * sizeof(VkLayerProperties));

    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers);

    for (const char* layerName : validationLayers)
    {
        bool layerFound = false;

        for (int j = 0; j < layerCount; j++)
        {
            if (strcmp(layerName, availableLayers[j].layerName) == 0)
            {
                layerFound = true;
                break;
            }
        }

        if (!layerFound)
        {
            free(availableLayers);
            return false;
        }
    }
    free(availableLayers);
    return true;
}

std::vector<const char*> getRequiredExtensions()
{
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);

    if (enableValidationLayers)
    {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#ifdef __APPLE__
        extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
#endif
    }
#ifdef __APPLE__
    uint32_t instanceExtensionCount;
    vkEnumerateInstanceExtensionProperties(nullptr, &instanceExtensionCount, nullptr);
    std::vector<VkExtensionProperties> availableInstanceExtensions(instanceExtensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &instanceExtensionCount, availableInstanceExtensions.data());
    for (const auto& extension : availableInstanceExtensions)
    {
        if (strcmp(extension.extensionName, VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME) == 0)
        {
            extensions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
            break;
        }
    }
#endif
    return extensions;
}

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData)
{
    std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
    // Try to extract more information about the error
    if (strstr(pCallbackData->pMessage, "renderPass is VK_NULL_HANDLE")) {
        std::cerr << "CUSTOM DEBUG: Detected renderPass NULL handle error!" << std::endl;
        // Print call stack or other debug info if possible
    }
    return VK_FALSE;
}


void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
{
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;
    createInfo.pUserData = nullptr; // Optional
}

void PrintGPUName(VkInstance instance)
{
    // Enumerate physical devices
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
    if (deviceCount == 0)
    {
        throw std::runtime_error("failed to find GPUs with Vulkan support!");
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

    // Print the name of each physical device
    for (const auto& device : devices)
    {
        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(device, &deviceProperties);
        std::cout << "GPU: " << deviceProperties.deviceName << std::endl;
    }
}



void CreateVKInstance(Renderer* renderer)
{
    if (enableValidationLayers && !checkValidationLayerSupport())
    {
        std::cerr << "Validation layers requested, but not available!" << std::endl;
        std::exit(EXIT_FAILURE);
    }

    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Hello Zayn";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "Zayn Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;

    createInfo.pApplicationInfo = &appInfo;

    auto extensions = getRequiredExtensions();
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = {};
    if (enableValidationLayers)
    {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();

        populateDebugMessengerCreateInfo(debugCreateInfo);
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
    }
    else
    {
        createInfo.enabledLayerCount = 0;
        createInfo.pNext = nullptr;
    }

    if (vkCreateInstance(&createInfo, nullptr, &renderer->data.vkInstance) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create instance!");
    }
    PrintGPUName(renderer->data.vkInstance);
}

VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger)
{
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr)
    {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    }
    else
    {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}


void SetupDebugMessenger(Renderer* renderer)
{
    if (!enableValidationLayers)
    {
        return;
    }

    VkDebugUtilsMessengerCreateInfoEXT createInfo;
    // populateDebugMessengerCreateInfo(createInfo);
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;
    createInfo.pUserData = nullptr; // Optional


    // VkAllocationCallbacks pAllocator = nullptr;

    if (CreateDebugUtilsMessengerEXT(renderer->data.vkInstance, reinterpret_cast<const VkDebugUtilsMessengerCreateInfoEXT*>(&createInfo), nullptr, &renderer->data.vkDebugMessenger) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to set up debug messenger!");
    }
#ifdef __APPLE__

#else
    if (CreateDebugUtilsMessengerEXT(renderer->data.vkInstance, reinterpret_cast<const VkDebugUtilsMessengerCreateInfoEXT*>(&createInfo), nullptr, &renderer->data.vkDebugMessenger) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to set up debug messenger!");
    }
#endif

}

void CreateSurface(Renderer* renderer, WindowManager* window)
{
    if (glfwCreateWindowSurface(renderer->data.vkInstance, window->glfwWindow, nullptr, &renderer->data.vkSurface) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create window surface!");
    }
}

bool checkDeviceExtensionSupport(VkPhysicalDevice device)
{
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

    std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

    for (const auto& extension : availableExtensions)
    {
        requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
}

SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device, Renderer* renderer)
{
    SwapChainSupportDetails details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, renderer->data.vkSurface, &details.capabilities);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, renderer->data.vkSurface, &formatCount, nullptr);

    if (formatCount != 0)
    {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, renderer->data.vkSurface, &formatCount, details.formats.data());
    }

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, renderer->data.vkSurface, &presentModeCount, nullptr);

    if (presentModeCount != 0)
    {
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, renderer->data.vkSurface, &presentModeCount, details.presentModes.data());
    }

    return details;
}

QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device, Renderer* renderer)
{
    QueueFamilyIndices indices;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    int i{};
    for (const auto& queueFamily : queueFamilies)
    {
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            indices.graphicsFamily = i;
        }

        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, renderer->data.vkSurface, &presentSupport);

        if (presentSupport)
        {
            indices.presentFamily = i;
        }

        if (indices.isComplete())
        {
            break;
        }

        i++;
    }

    return indices;
}


bool isDeviceSuitable(VkPhysicalDevice device, Renderer* renderer)
{
    QueueFamilyIndices indices = findQueueFamilies(device, renderer);
    bool extensionsSupported = checkDeviceExtensionSupport(device);

    bool swapChainAdequate = false;
    if (extensionsSupported)
    {
        SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device, renderer);
        swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
    }

    //VkPhysicalDeviceFeatures supportedFeatures;
    //vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

    //return indices.isComplete() && extensionsSupported && swapChainAdequate && supportedFeatures.samplerAnisotropy;
    return indices.isComplete() && extensionsSupported && swapChainAdequate;
}

void PickPhysicalDevice(Renderer* renderer)
{
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(renderer->data.vkInstance, &deviceCount, nullptr);

    if (deviceCount == 0)
    {
        throw std::runtime_error("failed to find GPUs with Vulkan support!");
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(renderer->data.vkInstance, &deviceCount, devices.data());

    for (const auto& device : devices)
    {
        // printDeviceExtensions(device);
        if (isDeviceSuitable(device, renderer))
        {
            renderer->data.vkPhysicalDevice = device;
            break;
        }
    }

    if (renderer->data.vkPhysicalDevice == VK_NULL_HANDLE)
    {
        throw std::runtime_error("failed to find a suitable GPU!");
    }
}

void CreateLogicalDevice(Renderer* renderer)
{
    QueueFamilyIndices indices = findQueueFamilies(renderer->data.vkPhysicalDevice, renderer);

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };

    float queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies)
    {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures = {};
    deviceFeatures.samplerAnisotropy = VK_TRUE;
    deviceFeatures.sampleRateShading = VK_TRUE;

    VkDeviceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pQueueCreateInfos = queueCreateInfos.data();

    createInfo.pEnabledFeatures = &deviceFeatures;

    createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
    createInfo.ppEnabledExtensionNames = deviceExtensions.data();

    if (enableValidationLayers)
    {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
    }
    else
    {
        createInfo.enabledLayerCount = 0;
    }

    if (vkCreateDevice(renderer->data.vkPhysicalDevice, &createInfo, nullptr, &renderer->data.vkDevice) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create logical device!");
    }

    vkGetDeviceQueue(renderer->data.vkDevice, indices.graphicsFamily.value(), 0, &renderer->data.vkGraphicsQueue);
    vkGetDeviceQueue(renderer->data.vkDevice, indices.presentFamily.value(), 0,  &renderer->data.vkPresentQueue);
}

VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
    for (const auto& availableFormat : availableFormats)
    {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            return availableFormat;
        }
    }

    return availableFormats[0];
}

VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
{
    std::cout << "Present mode: V-Sync?" << std::endl;

    for (const auto& availablePresentMode : availablePresentModes)
    {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
        {
            std::cout << "Present mode: Mailbox" << std::endl;
            return availablePresentMode;
        }
        // UNCOMMENT BELOW IF YOU WANT TO NOT HAVE VSYNC ON
         /*else if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR)
         {
             std::cout << "Present mode: Immediate (No VSync)" << std::endl;
             return availablePresentMode;
         }*/
    }
    return VK_PRESENT_MODE_FIFO_KHR;
}
VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, WindowManager* window)
{
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
    {
        return capabilities.currentExtent;
    }
    else
    {
        int width, height;
        glfwGetFramebufferSize(window->glfwWindow, &width, &height);

        VkExtent2D actualExtent =
                {
                        static_cast<uint32_t>(width),
                        static_cast<uint32_t>(height)
                };

        actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

        return actualExtent;
    }
}



void CreateSwapChain(Renderer* renderer, WindowManager* window)
{
    SwapChainSupportDetails swapChainSupport = querySwapChainSupport(renderer->data.vkPhysicalDevice, renderer);

    VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapChainSupport.presentModes);
    VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities, window);

    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
    if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = renderer->data.vkSurface;

    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    QueueFamilyIndices indices = findQueueFamilies(renderer->data.vkPhysicalDevice, renderer);
    uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

    if (indices.graphicsFamily != indices.presentFamily) {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }

    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;

    createInfo.oldSwapchain = VK_NULL_HANDLE;

    if (vkCreateSwapchainKHR(renderer->data.vkDevice, &createInfo, nullptr, &renderer->data.vkSwapChain) != VK_SUCCESS) {
        throw std::runtime_error("failed to create swap chain!");
    }

    vkGetSwapchainImagesKHR(renderer->data.vkDevice, renderer->data.vkSwapChain, &imageCount, nullptr);
    renderer->data.vkSwapChainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(renderer->data.vkDevice, renderer->data.vkSwapChain, &imageCount, renderer->data.vkSwapChainImages.data());

    renderer->data.vkSwapChainImageFormat = surfaceFormat.format;
    renderer->data.vkSwapChainExtent = extent;
}

void CreateImageViews(Renderer* renderer)
{
    renderer->data.vkSwapChainImageViews.resize(renderer->data.vkSwapChainImages.size());

    for (size_t i = 0; i < renderer->data.vkSwapChainImages.size(); i++)
    {
        VkImageViewCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = renderer->data.vkSwapChainImages[i];
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = renderer->data.vkSwapChainImageFormat;
        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;

        if (vkCreateImageView(renderer->data.vkDevice, &createInfo, nullptr, &renderer->data.vkSwapChainImageViews[i]) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create image views!");
        }
    }
}

VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features, Renderer* renderer)
{
    for (VkFormat format : candidates)
    {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(renderer->data.vkPhysicalDevice, format, &props);

        if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)
        {
            return format;
        }
        else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features)
        {
            return format;
        }
    }

    throw std::runtime_error("failed to find supported format!");
}



VkFormat FindDepthFormat(Renderer* renderer)
{
    return FindSupportedFormat(
            { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
            VK_IMAGE_TILING_OPTIMAL,
            VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT, renderer);
}

void CreateRenderPass(Renderer* renderer)
{

    std::cout << "Starting render pass creation..." << std::endl;

    // Check if device is valid
    if (renderer->data.vkDevice == VK_NULL_HANDLE) {
        std::cerr << "ERROR: vkDevice is NULL when creating render pass!" << std::endl;
        return;
    }

    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = renderer->data.vkSwapChainImageFormat;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;


    // changed for imgui
#ifdef IMGUI
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
#else
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
#endif



    VkAttachmentDescription depthAttachment{};
    depthAttachment.format = FindDepthFormat(renderer);
    depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthAttachmentRef{};
    depthAttachmentRef.attachment = 1;
    depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;
    subpass.pDepthStencilAttachment = &depthAttachmentRef;

    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    std::array<VkAttachmentDescription, 2> attachments = { colorAttachment, depthAttachment };
    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    renderPassInfo.pAttachments = attachments.data();
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    if (vkCreateRenderPass(renderer->data.vkDevice, &renderPassInfo, nullptr, &renderer->data.vkRenderPass) != VK_SUCCESS)
    {
        std::cerr << "ERROR: Failed to create render pass!" << std::endl;
        throw std::runtime_error("failed to create render pass!");
    }
    else {
        std::cout << "Render pass created successfully. Handle: " << renderer->data.vkRenderPass << std::endl;
    }

#if IMGUI


    // Attachment
    colorAttachment = {};
    colorAttachment.format = renderer->data.vkSwapChainImageFormat;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT; // No MSAA
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    // Color VkAttachmentReference our render pass needs.
    colorAttachmentRef = {};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    // subpass
    subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;

    // synchronization and dependency
    dependency = {};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0; // or VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    info.attachmentCount = 1;
    info.pAttachments = &colorAttachment;
    info.subpassCount = 1;
    info.pSubpasses = &subpass;
    info.dependencyCount = 1;
    info.pDependencies = &dependency;
    if (vkCreateRenderPass(renderer->data.vkDevice, &info, nullptr, &renderer->myImgui.imGuiRenderPass) !=
        VK_SUCCESS) {
        throw std::runtime_error("Could not create Dear ImGui's render pass");
    }

#endif
}

void CreateCommandPool(Renderer* renderer)
{
    QueueFamilyIndices queueFamilyIndices = findQueueFamilies(renderer->data.vkPhysicalDevice, renderer);

    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    //poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

    if (vkCreateCommandPool(renderer->data.vkDevice, &poolInfo, nullptr, &renderer->data.vkCommandPool) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create command pool!");
    }

#if IMGUI

    if (vkCreateCommandPool(renderer->data.vkDevice, &poolInfo, nullptr, &renderer->myImgui.imGuiCommandPool) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create command pool!");
    }

#endif
}

uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties, Renderer* renderer)
{
    // VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(renderer->data.vkPhysicalDevice, &renderer->data.vkMemProperties);
    for (uint32_t i = 0; i < renderer->data.vkMemProperties.memoryTypeCount; i++)
    {
        if ((typeFilter & (1 << i)) && (renderer->data.vkMemProperties.memoryTypes[i].propertyFlags & properties) == properties)
        {
            return i;
        }
    }
    throw std::runtime_error("failed to find suitable memory type!");
}

void CreateImage(uint32_t width,
                 uint32_t height,
                 uint32_t mipLevels,
                 VkFormat format,
                 VkImageTiling tiling,
                 VkImageUsageFlags usage,
                 VkMemoryPropertyFlags properties,
                 VkImage& image,
                 VkDeviceMemory& imageMemory,
                 Renderer* renderer)
{
    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = width;
    imageInfo.extent.height = height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = mipLevels;
    imageInfo.arrayLayers = 1;
    imageInfo.format = format;
    imageInfo.tiling = tiling;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = usage;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateImage(renderer->data.vkDevice, &imageInfo, nullptr, &image) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create image!");
    }

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(renderer->data.vkDevice, image, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, properties, renderer);

    if (vkAllocateMemory(renderer->data.vkDevice, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to allocate image memory!");
    }

    vkBindImageMemory(renderer->data.vkDevice, image, imageMemory, 0);
}


VkCommandBuffer BeginSingleTimeCommands(Renderer* renderer)
{
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = renderer->data.vkCommandPool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(renderer->data.vkDevice, &allocInfo, &commandBuffer);

#if IMGUI
    renderer->myImgui.imGuiCommandBuffers.resize(
    renderer->data.vkSwapChainImageViews.size());

    allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = renderer->myImgui.imGuiCommandPool;
    allocInfo.commandBufferCount =
    static_cast<uint32_t>(renderer->myImgui.imGuiCommandBuffers.size());
    vkAllocateCommandBuffers(renderer->data.vkDevice, &allocInfo, renderer->myImgui.imGuiCommandBuffers.data());

#endif

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    return commandBuffer;
}

void EndSingleTimeCommands(Renderer* renderer, VkCommandBuffer commandBuffer)
{
    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(renderer->data.vkGraphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(renderer->data.vkGraphicsQueue);

    vkFreeCommandBuffers(renderer->data.vkDevice, renderer->data.vkCommandPool, 1, &commandBuffer);
}

void TransitionImageLayout(Renderer* renderer, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels)
{
    VkCommandBuffer commandBuffer = BeginSingleTimeCommands(renderer);

    VkImageMemoryBarrier barrier = {};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

    barrier.image = image;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = mipLevels;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    VkPipelineStageFlags sourceStage;
    VkPipelineStageFlags destinationStage;

    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
    {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        //destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = (renderer->data.vkGraphicsQueue) ? VK_PIPELINE_STAGE_TRANSFER_BIT : VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
    {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    else
    {
        throw std::invalid_argument("unsupported layout transition!");
    }

    vkCmdPipelineBarrier(
            commandBuffer,
            sourceStage, destinationStage,
            0,
            0, nullptr,
            0, nullptr,
            1, &barrier);

    EndSingleTimeCommands(renderer, commandBuffer);
}

void GenerateMipmaps(Renderer* renderer, VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels)
{
    // Check if image format supports linear blitting
    VkFormatProperties formatProperties;
    vkGetPhysicalDeviceFormatProperties(renderer->data.vkPhysicalDevice, imageFormat, &formatProperties);

    if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT))
    {
        throw std::runtime_error("texture image format does not support linear blitting!");
    }

    VkCommandBuffer commandBuffer = BeginSingleTimeCommands(renderer);

    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.image = image;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;
    barrier.subresourceRange.levelCount = 1;

    int32_t mipWidth = texWidth;
    int32_t mipHeight = texHeight;

    for (uint32_t i = 1; i < mipLevels; i++)
    {
        barrier.subresourceRange.baseMipLevel = i - 1;
        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

        vkCmdPipelineBarrier(commandBuffer,
                             VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
                             0, nullptr,
                             0, nullptr,
                             1, &barrier);

        VkImageBlit blit{};
        blit.srcOffsets[0] = { 0, 0, 0 };
        blit.srcOffsets[1] = { mipWidth, mipHeight, 1 };
        blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        blit.srcSubresource.mipLevel = i - 1;
        blit.srcSubresource.baseArrayLayer = 0;
        blit.srcSubresource.layerCount = 1;
        blit.dstOffsets[0] = { 0, 0, 0 };
        blit.dstOffsets[1] = { mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1 };
        blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        blit.dstSubresource.mipLevel = i;
        blit.dstSubresource.baseArrayLayer = 0;
        blit.dstSubresource.layerCount = 1;

        vkCmdBlitImage(commandBuffer,
                       image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                       image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                       1, &blit,
                       VK_FILTER_LINEAR);

        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        vkCmdPipelineBarrier(commandBuffer,
                             VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
                             0, nullptr,
                             0, nullptr,
                             1, &barrier);

        if (mipWidth > 1)
            mipWidth /= 2;
        if (mipHeight > 1)
            mipHeight /= 2;
    }

    barrier.subresourceRange.baseMipLevel = mipLevels - 1;
    barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

    vkCmdPipelineBarrier(commandBuffer,
                         VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
                         0, nullptr,
                         0, nullptr,
                         1, &barrier);

    EndSingleTimeCommands(renderer, commandBuffer);
}

void CopyBufferToImage(Renderer* renderer, VkBuffer buffer, VkImage image, uint32_t width, uint32_t height)
{
    VkCommandBuffer commandBuffer = BeginSingleTimeCommands(renderer);

    VkBufferImageCopy region = {};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;

    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;

    region.imageOffset = { 0, 0, 0 };
    region.imageExtent = {
            width,
            height,
            1 };

    vkCmdCopyBufferToImage(
            commandBuffer,
            buffer,
            image,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            1,
            &region);

    EndSingleTimeCommands(renderer, commandBuffer);
}



VkImageView CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels, Renderer* renderer)
{
    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = image;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = format;
    viewInfo.subresourceRange.aspectMask = aspectFlags;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = mipLevels;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    VkImageView imageView;
    if (vkCreateImageView(renderer->data.vkDevice, &viewInfo, nullptr, &imageView) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create texture image view!");
    }

    return imageView;
}


void CreateDepthResources(Renderer* renderer)
{
    VkFormat depthFormat = FindDepthFormat(renderer);

    CreateImage(renderer->data.vkSwapChainExtent.width, renderer->data.vkSwapChainExtent.height, 1, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, renderer->data.vkDepthImage, renderer->data.vkDepthImageMemory, renderer);
    renderer->data.vkDepthImageView = CreateImageView(renderer->data.vkDepthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, 1, renderer);
}


void CreateFrameBuffers(Renderer* renderer)
{
    if (!renderer) {
        std::cerr << "ERROR: renderer is NULL!" << std::endl;
        return;
    }

    std::cout << "Starting framebuffer creation..." << std::endl;
    std::cout << "renderer address: " << renderer << std::endl;
    std::cout << "vkRenderPass handle in CreateFrameBuffers: " << renderer->data.vkRenderPass << std::endl;

    std::cout << "Framebuffers vector size before resize: "
              << renderer->data.vkSwapChainFramebuffers.size() << std::endl;

    // Check if render pass is valid
    if (renderer->data.vkRenderPass == VK_NULL_HANDLE) {
        std::cerr << "ERROR: vkRenderPass is NULL when creating framebuffers!" << std::endl;
        return;
    }
    renderer->data.vkSwapChainFramebuffers.resize(renderer->data.vkSwapChainImageViews.size());
    std::cout << "Resized framebuffers vector to: "
              << renderer->data.vkSwapChainFramebuffers.size() << std::endl;


#if IMGUI

    renderer->myImgui.imGuiFrameBuffers.resize(renderer->data.vkSwapChainImageViews.size());

#endif

    for (size_t i = 0; i < renderer->data.vkSwapChainImageViews.size(); i++)
    {
        std::cout << "Creating framebuffer " << i << "..." << std::endl;

        std::array<VkImageView, 2> attachments = {
                renderer->data.vkSwapChainImageViews[i],
                renderer->data.vkDepthImageView
        };

        std::cout << "Attachments: " << attachments[0] << ", " << attachments[1] << std::endl;

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = renderer->data.vkRenderPass;

        std::cout << "Using vkRenderPass in framebufferInfo: " << framebufferInfo.renderPass << std::endl;

        // Print the rest of framebufferInfo for debugging
        framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        framebufferInfo.pAttachments = attachments.data();
        framebufferInfo.width = renderer->data.vkSwapChainExtent.width;
        framebufferInfo.height = renderer->data.vkSwapChainExtent.height;
        framebufferInfo.layers = 1;

        std::cout << "Before vkCreateFramebuffer call..." << std::endl;
        VkResult result = vkCreateFramebuffer(
                renderer->data.vkDevice,
                &framebufferInfo,
                nullptr,
                &renderer->data.vkSwapChainFramebuffers[i]
        );
        std::cout << "vkCreateFramebuffer result: " << result << std::endl;

        if (result != VK_SUCCESS) {
            std::cerr << "ERROR creating framebuffer " << i << ": " << result << std::endl;
            throw std::runtime_error("failed to create framebuffer!");
        }

        std::cout << "Framebuffer " << i << " created successfully." << std::endl;


#if IMGUI

        // Imgui framebuffer
        VkImageView imgui_attachment[1];
        framebufferInfo = {};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = renderer->myImgui.imGuiRenderPass;
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments = imgui_attachment;
        framebufferInfo.width = renderer->data.vkSwapChainExtent.width;
        framebufferInfo.height = renderer->data.vkSwapChainExtent.height;
        framebufferInfo.layers = 1;
        imgui_attachment[0] = renderer->data.vkSwapChainImageViews[i];
        if (vkCreateFramebuffer(renderer->data.vkDevice, &framebufferInfo, nullptr,
            &renderer->myImgui.imGuiFrameBuffers[i]) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create ImGui framebuffer!");
        }

#endif
    }
}


void StartRender_Init(Renderer* renderer, WindowManager* window)
{
    CreateVKInstance(renderer);
    SetupDebugMessenger(renderer);
    CreateSurface(renderer, window);
    PickPhysicalDevice(renderer);    // <-- @TODO: May need to modify to account for other machines like rPi
    CreateLogicalDevice(renderer);   // is SAME

    CreateSwapChain(renderer, window);
    CreateImageViews(renderer);

    CreateRenderPass(renderer);

    CreateCommandPool(renderer);
    CreateDepthResources(renderer);
    CreateFrameBuffers(renderer);
}





void CopyBuffer(Renderer* renderer, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
{
    VkCommandBuffer commandBuffer = BeginSingleTimeCommands(renderer);

    VkBufferCopy copyRegion{};
    copyRegion.size = size;
    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

    EndSingleTimeCommands(renderer, commandBuffer);
}




void CreateBuffer(Renderer* renderer, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory)
{
    VkBufferCreateInfo bufferInfo = {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage | VK_BUFFER_CREATE_SPARSE_BINDING_BIT;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(renderer->data.vkDevice, &bufferInfo, nullptr, &buffer) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create buffer!");
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(renderer->data.vkDevice, buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, properties, renderer);

    if (vkAllocateMemory(renderer->data.vkDevice, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to allocate buffer memory!");
    }

    vkBindBufferMemory(renderer->data.vkDevice, buffer, bufferMemory, 0);
}


void CreateDescriptorSetLayout(Renderer* renderer, VkDescriptorSetLayout* descriptorSetLayout, bool hasImage)
{
    std::vector<VkDescriptorSetLayoutBinding> bindings = {};
    VkDescriptorSetLayoutBinding uboLayoutBinding{};
    uboLayoutBinding.binding = 0;
    uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uboLayoutBinding.descriptorCount = 1;
    uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    bindings.push_back(uboLayoutBinding);

    if (hasImage)
    {
        VkDescriptorSetLayoutBinding samplerLayoutBinding{};
        samplerLayoutBinding.binding = 1;
        samplerLayoutBinding.descriptorCount = 1;
        samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        samplerLayoutBinding.pImmutableSamplers = nullptr;
        samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
        bindings.push_back(samplerLayoutBinding);
    }

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
    layoutInfo.pBindings = bindings.data();

    if (vkCreateDescriptorSetLayout(renderer->data.vkDevice, &layoutInfo, nullptr, descriptorSetLayout) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create descriptor set layout!");
    }
}

void CreateDescriptorPool(Renderer* renderer, VkDescriptorPool* descriptorPool, bool hasImage)
{
    std::vector<VkDescriptorPoolSize> poolSizes = {};
    VkDescriptorPoolSize poolSize_1{};
    poolSize_1.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSize_1.descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT * 20);
    poolSizes.push_back(poolSize_1);
    if (hasImage)
    {
        VkDescriptorPoolSize poolSize_2 = {};
        poolSize_2.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        poolSize_2.descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT * 20);
        poolSizes.push_back(poolSize_2);
    }

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT * 20);

    if (vkCreateDescriptorPool(renderer->data.vkDevice, &poolInfo, nullptr, descriptorPool) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create descriptor pool!");
    }
}

VkShaderModule CreateShaderModule(Renderer* renderer, const std::vector<char>& code)
{
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

    VkShaderModule shaderModule;
    if (vkCreateShaderModule(renderer->data.vkDevice, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create shader module!");
    }

    return shaderModule;
}

static std::vector<char> ReadFile(const std::string& filename)
{
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open())
    {
        throw std::runtime_error("failed to open file!");
    }

    size_t fileSize = (size_t)file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();

    return buffer;
}


template <typename T>
void CreatePushConstant(Renderer* renderer)
{
    VkPushConstantRange pushConstantRange = {};
    pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    pushConstantRange.offset = 0;
    pushConstantRange.size = sizeof(T);

    renderer->data.vkPushConstantRanges.push_back(pushConstantRange);
}

void  CreateGraphicsPipeline(Renderer* renderer, VkPipeline* pipeline, const std::string& vertShaderFilePath, const std::string& fragShaderFilePath, std::vector<VkPushConstantRange> pushConstants, VkDescriptorSetLayout* descriptorSetLayout, VkPipelineLayout* pipelineLayout)
{
    auto vertShaderCode = ReadFile(vertShaderFilePath);
    auto fragShaderCode = ReadFile(fragShaderFilePath);

    VkShaderModule vertShaderModule = CreateShaderModule(renderer, vertShaderCode);
    VkShaderModule fragShaderModule = CreateShaderModule(renderer, fragShaderCode);

    VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vertShaderModule;
    vertShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = fragShaderModule;
    fragShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

    auto bindingDescription = Vertex::getBindingDescription();
    auto attributeDescriptions = Vertex::getAttributeDescriptions();

    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
    vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    VkPipelineViewportStateCreateInfo viewportState = {};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.scissorCount = 1;

    VkPipelineRasterizationStateCreateInfo rasterizer = {};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;

    VkPipelineMultisampleStateCreateInfo multisampling = {};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    VkPipelineDepthStencilStateCreateInfo depthStencil = {};
    depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencil.depthTestEnable = VK_TRUE;
    depthStencil.depthWriteEnable = VK_TRUE;
    depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
    depthStencil.depthBoundsTestEnable = VK_FALSE;
    depthStencil.minDepthBounds = 0.0f; // Optional
    depthStencil.maxDepthBounds = 1.0f; // Optional

    depthStencil.stencilTestEnable = VK_FALSE;
    depthStencil.front = {}; // Optional
    depthStencil.back = {};  // Optional

    VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;

    VkPipelineColorBlendStateCreateInfo colorBlending = {};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f;
    colorBlending.blendConstants[1] = 0.0f;
    colorBlending.blendConstants[2] = 0.0f;
    colorBlending.blendConstants[3] = 0.0f;

    std::vector<VkDynamicState> dynamicStates = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR };

    VkPipelineDynamicStateCreateInfo dynamicState = {};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    dynamicState.pDynamicStates = dynamicStates.data();

    VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = descriptorSetLayout;

    VkPushConstantRange pushConstantRange = {};
    pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    pushConstantRange.offset = 0;
    pushConstantRange.size = sizeof(ModelPushConstant);
    pipelineLayoutInfo.pushConstantRangeCount = 1;
    pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

    if (vkCreatePipelineLayout(renderer->data.vkDevice, &pipelineLayoutInfo, nullptr, pipelineLayout) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create pipeline layout!");
    }

    VkGraphicsPipelineCreateInfo pipelineInfo = {};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pDepthStencilState = &depthStencil;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDynamicState = &dynamicState;
    pipelineInfo.layout = *pipelineLayout;
    pipelineInfo.renderPass = renderer->data.vkRenderPass;
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

    if (vkCreateGraphicsPipelines(renderer->data.vkDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, pipeline) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create graphics pipeline!");
    }

    vkDestroyShaderModule(renderer->data.vkDevice, fragShaderModule, nullptr);
    vkDestroyShaderModule(renderer->data.vkDevice, vertShaderModule, nullptr);
}

std::string GetShaderPath(const std::string& filename) {
#ifdef WINDOWS
    return "../src/render/shaders/compiled/" + filename;
    //return "/src/renderer/shaders/" + filename;
#else
    // return "/Users/socki/dev/zayn2/src/renderer/shaders/" + filename;
    return "/Users/adamsocki/dev/clion/z1000/src/managers/render/shaders/compiled/" + filename;
#endif
}

void CreateUniformBuffer(Renderer* renderer, std::vector<VkBuffer>& uniformBuffers, std::vector<VkDeviceMemory>& uniformBuffersMemory, std::vector<void*>& uniformBuffersMapped)
{
    VkDeviceSize bufferSize = sizeof(UniformBufferObject);

    uniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
    uniformBuffersMemory.resize(MAX_FRAMES_IN_FLIGHT);
    uniformBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        CreateBuffer(renderer, bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uniformBuffers[i], uniformBuffersMemory[i]);
        vkMapMemory(renderer->data.vkDevice, uniformBuffersMemory[i], 0, bufferSize, 0, &uniformBuffersMapped[i]);
    }
}

void CreateCommandBuffers(Renderer* renderer)
{
    // Zayn->vkCommandBuffers = (VkCommandBuffer *)malloc(sizeof(VkCommandBuffer) * MAX_FRAMES_IN_FLIGHT);
    renderer->data.vkCommandBuffers.resize(MAX_FRAMES_IN_FLIGHT);

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = renderer->data.vkCommandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = MAX_FRAMES_IN_FLIGHT;

    if (vkAllocateCommandBuffers(renderer->data.vkDevice, &allocInfo, renderer->data.vkCommandBuffers.data()) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to allocate command buffers!");
    }
#if IMGUI
    // ImGui Command Buffer
    //zaynMem->imGuiCommandBuffers.resize(zaynMem->vkSwapChainImageViews.size()); // or MAX_FRAMES_IN_FLIGHT?
    renderer->myImgui.imGuiCommandBuffers.resize(MAX_FRAMES_IN_FLIGHT);
    // create command buffers
    allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = renderer->myImgui.imGuiCommandPool;
    allocInfo.commandBufferCount =
        static_cast<uint32_t>(renderer->myImgui.imGuiCommandBuffers.size());
    vkAllocateCommandBuffers(renderer->data.vkDevice, &allocInfo, renderer->myImgui.imGuiCommandBuffers.data());
#endif
}

void CreateSyncObjects(Renderer* renderer)
{
    renderer->data.vkImageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    size_t imageCount = renderer->data.vkSwapChainImages.size();
    renderer->data.vkRenderFinishedSemaphores.resize(imageCount);

    renderer->data.vkInFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
    renderer->data.vkImagesInFlight.resize(imageCount, VK_NULL_HANDLE);

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;


    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        if (vkCreateSemaphore(renderer->data.vkDevice, &semaphoreInfo, nullptr,
                             &renderer->data.vkImageAvailableSemaphores[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create image available semaphores!");
                             }
    }

    // Create in-flight fences
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        if (vkCreateFence(renderer->data.vkDevice, &fenceInfo, nullptr,
                         &renderer->data.vkInFlightFences[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create in-flight fences!");
                         }
    }

    // Existing render finished semaphore creation
    for (size_t i = 0; i < renderer->data.vkSwapChainImages.size(); i++) {
        if (vkCreateSemaphore(renderer->data.vkDevice, &semaphoreInfo, nullptr,
                             &renderer->data.vkRenderFinishedSemaphores[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create render finished semaphores!");
                             }
    }
}


void InitRender_Vulkan(Renderer* renderer, WindowManager* window)
{
    std::cout << "InitRender_Vulkan()" << std::endl;
    std::cout << "InitRender_Vulkan() with renderer address: " << renderer << std::endl;

    // I DONT THINK ANY GAME SPECIFIC THINGS OCCUR HERE

    StartRender_Init(renderer, window);

#if IMGUI
    InitMyImgui(renderer, window);



#endif
    std::cout << "after InitRender_Vulkan()" << std::endl;

// INIT MATERIAL SYSTEM

    //zaynMem->materialSystem = new MaterialSystem(&zaynMem->vulkan.vkDevice);

    CreateDescriptorSetLayout(renderer, &renderer->data.vkDescriptorSetLayout, true); // this one is for those that have texures attached
    CreateDescriptorPool(renderer, &renderer->data.vkDescriptorPool, true);      // <---- CAN POTENTIAL BE RESUSED BETWEEN ENTITIES THAT HAVE THE SAME TYPES OF THINGS BEING SHARED

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
            renderer->data.vkImageAvailableSemaphores[renderer->data.vkCurrentFrame], // must be a not signaled semaphore
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

    // vkResetFences(Zayn->vkDevice, 1, &Zayn->vkInFlightFences[Zayn->vkCurrentFrame]);
    // vkResetCommandBuffer(Zayn->vkCommandBuffers[Zayn->vkCurrentFrame], /*VkCommandBufferResetFlagBits*/ 0);
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
    // static auto startTime = std::chrono::high_resolution_clock::now();

    // auto currentTime = std::chrono::high_resolution_clock::now();
    // float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();


//    Camera* cam = cam;

    UniformBufferObject ubo = {};
    // ubo.model = TRS(V3(0.0f, 0.0f, 0.0f), AxisAngle(V3(0.0f, 0.2f, 0.20f), time * DegToRad(10.0f)), V3(1.0f, 1.0f, 1.0f));

    // apply view based on camera rotation

    glm::vec3 camPos = glm::vec3(cam->pos.x, cam->pos.y, cam->pos.z);
    glm::vec3 camFront = glm::vec3(cam->front.x, cam->front.y, cam->front.z);
    glm::vec3 camUp = glm::vec3(cam->up.x, cam->up.y, cam->up.z);

    ubo.view = glm::lookAt(camPos, camPos + camFront, camUp);
    // ubo.view = glm::lookAt(cam->pos, cam->pos + cam->front, cam->up);

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
// namespace Zayn {
//     namespace LE {
//         void RenderEntities(Engine* engine, VkCommandBuffer commandBuffer)
//         {
//             //uint32_t dynamicOffset = zaynMem->vulkan.vkCurrentFrame * sizeof(UniformBufferObject);
//             // PlayerEntity* playerEntity = (PlayerEntity*)GetEntity(&engine->entityFactory, engine->HTEST);

//             // for (int i = 0; i < )

//             // can't do it if there is no transform or render component

//             ComponentFactory * componentfactory = &engine->componentFactory;
//             ComponentStorage* storage = &componentfactory->componentStorage;

//             EntityFactory* entityFactory = &engine->entityFactory;


//             for (int i = 0; i < storage->renderComponents.count; i++) {
//                 RenderComponent* rc = &storage->renderComponents[i];

//                 Entity* entity = static_cast<Entity*>(GetEntity(entityFactory, rc->owner));


//                // //    Check the pointer AND potentially the buffer handles within the mesh struct.
//                 if (rc->mesh == nullptr ||
//                   rc->mesh->vertexBuffer == VK_NULL_HANDLE ||
//                 rc->mesh->indexBuffer == VK_NULL_HANDLE ||
//                 rc->mesh->indices.empty()) // Also check if there are indices to draw
//                 {
//                   // No valid mesh data to render.
//                     // Optional: Log a warning if this state is unexpected for a visible component.
//                     // printf("Warning: Skipping entity (Idx:%d): RenderComponent has null or invalid mesh data.\n", rc->owner.indexInInfo);
//                     continue; // Skip to the next component
//                 }
//                 TransformComponent* tc = FindComponentInArray(&storage->transformComponents, rc->owner);
//                 if (!tc) {
//                     // Has mesh data but no position/orientation/scale. Cannot place it in the world.
//                     // Optional: Log a warning.
//                     // printf("Warning: Skipping entity (Idx:%d): RenderComponent has mesh but no TransformComponent.\n", rc->owner.indexInInfo);
//                     continue; // Skip to the next component
//                 }

//                     rc->transformComponentAvailable = true;

//                 if (entity) {

//                     // VkDescriptorSet& set = entity->material->descriptorSets[engine->renderer.data.vkCurrentFrame];
//                     VkDescriptorSet& set = rc->material->descriptorSets[engine->renderer.data.vkCurrentFrame];

//                     vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, engine->renderer.data.vkGraphicsPipeline);
//                     vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, engine->renderer.data.vkPipelineLayout, 0, 1, &set, 0, nullptr);


//                     // Push constants for the transform
//                     /* ModelPushConstant pushConstant = {};
//                      pushConstant.model_1 = TRS((V3(0.0f, 1.0f, -1.0f)), AxisAngle(V3(0.0f, 0.2f, 0.20f), 0.0f), V3(1.0f, 1.0f, 1.0f));*/

//                     vkCmdPushConstants(commandBuffer, engine->renderer.data.vkPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(ModelPushConstant), &entity->pushConstantData);

//                     // Bind the vertex and index buffers
//                     VkBuffer vertexBuffers[] = { rc->mesh->vertexBuffer };
//                     VkDeviceSize offsets[] = { 0 };
//                     vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

//                     vkCmdBindIndexBuffer(commandBuffer, rc->mesh->indexBuffer, 0, VK_INDEX_TYPE_UINT32);

//                     // Draw the mesh
//                     vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(rc->mesh->indices.size()), 1, 0, 0, 0);

//                 }

//             }


//         }
//     }
// }

void RenderEntities(Zayn* zaynMem, VkCommandBuffer commandBuffer)
{
    // uint32_t dynamicOffset = zaynMem->renderer.data.vkCurrentFrame * sizeof(UniformBufferObject);
    // PlayerEntity* playerEntity = (PlayerEntity*)GetEntity(&zaynMem->entityFactory, engine->HTEST);

    EntityFactory* entityFactory = &zaynMem->entityFactory;
    ComponentsFactory * componentfactory = &zaynMem->componentsFactory;

    // for (int i = 0; i < storage->renderComponents.count; i++)

    // for (int i = 0; i < zaynMem->entityFactory.entities.count; i++)
    {
       // // GameObject& gameObj = zaynMem->gameObjects[i];
       //  // VkDescriptorSet& set = playerEntity->material->descriptorSets[zaynMem->renderer.data.vkCurrentFrame];
       //
       //  vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, zaynMem->renderer.data.vkGraphicsPipeline);
       //  vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, zaynMem->renderer.data.vkPipelineLayout, 0, 1, &set, 0, nullptr);
       //
       //
       //  // Push constants for the transform
       //  /* ModelPushConstant pushConstant = {};
       //   pushConstant.model_1 = TRS((V3(0.0f, 1.0f, -1.0f)), AxisAngle(V3(0.0f, 0.2f, 0.20f), 0.0f), V3(1.0f, 1.0f, 1.0f));*/
       //
       //  vkCmdPushConstants(commandBuffer, zaynMem->renderer.data.vkPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(ModelPushConstant), &playerEntity->pushConstantData);
       //
       //  // Bind the vertex and index buffers
       //  VkBuffer vertexBuffers[] = { playerEntity->mesh->vertexBuffer };
       //  VkDeviceSize offsets[] = { 0 };
       //  vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
       //
       //  vkCmdBindIndexBuffer(commandBuffer, playerEntity->mesh->indexBuffer, 0, VK_INDEX_TYPE_UINT32);
       //
       //  // Draw the mesh
       //  vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(playerEntity->mesh->indices.size()), 1, 0, 0, 0);
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
    // submitInfo.commandBufferCount
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
    // presentInfo.pImageIndices = &Zayn->vkCurrentImageIndex;

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
    // Only include ImGui command buffer if ImGui's visible (command buffer was recorded)
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

        // if (engine->inLevelEditor) {
        if (false) {
            // LE::RenderEntities(engine, renderer->data.vkCommandBuffers[renderer->data.vkCurrentFrame]);
        }
        else {
            RenderEntities(zaynMem, renderer->data.vkCommandBuffers[renderer->data.vkCurrentFrame]);
        }
        
        // Render collision shapes in 3D if enabled
        // if (engine->physicsManager.use3DColliders) {
            // RenderColliders3D(zaynMem, renderer);
        // }

#if IMGUI
        UpdateMyImgui(engine, &engine->levelEditor, camera, renderer, windowManager, inputManager);
#endif


    }
    EndSwapChainRenderPass(renderer, renderer->data.vkCommandBuffers[renderer->data.vkCurrentFrame]);
    EndFrameRender(renderer, windowManager);
}