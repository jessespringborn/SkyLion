//
// Created by jesse on 2/6/24.
//

#include "SkyLion.h"

#ifndef NDEBUG
bool isValidationEnabled = true;
#else
bool isValidationEnabled = false;
#endif

// vulkan objects
VkInstance g_vulkanInstance;
VkDebugUtilsMessengerEXT g_vulkanDebugMessenger;
VkPhysicalDevice g_vulkanPhysicalDevice;
VkDevice g_vulkanDevice;
VkQueue g_vulkanGraphicsQueue;
VkQueue g_vulkanPresentQueue;
VkSurfaceKHR g_vulkanSurface;
uint32_t g_vulkanValidationLayerCount = 1;
const char* g_vulkanValidationLayers[] = {"VK_LAYER_KHRONOS_validation"};
uint32_t g_vulkanGraphicsFamily;
uint32_t g_vulkanPresentFamily;
VkSurfaceFormatKHR* g_vulkanSurfaceFormats;
VkPresentModeKHR* g_vulkanPresentModes;
VkSurfaceCapabilitiesKHR g_vulkanSurfaceCapabilities;
uint32_t g_vulkanSurfaceFormatCount;
uint32_t g_vulkanPresentModeCount;
VkPhysicalDeviceProperties g_vulkanPhysicalDeviceProperties;
VkPhysicalDeviceMemoryProperties g_vulkanPhysicalDeviceMemoryProperties;
VkPhysicalDeviceFeatures g_vulkanDeviceFeatures;
VkCommandPool g_vulkanCommandPool;
uint32_t g_vulkanImageCount;
VkSwapchainKHR g_vulkanSwapChain;
VkImageView *g_vulkanImageViews;
VkImage* g_vulkanImages;
VkExtent2D g_vulkanSwapExtent;
VkFormat g_vulkanImageFormat;
VkDescriptorPool g_vulkanDescriptorPool;
VkDescriptorSetLayout g_vulkanUniformBufferLayout;
VkDescriptorSetLayout g_vulkanTextureLayout;
VkDescriptorSetLayout* g_vulkanLayouts;
uint32_t g_vulkanLayoutsCount = 2;
VkSampler g_vulkanTextureSampler;
VkFormat g_vulkanDepthFormat;
VkRenderPass g_vulkanRenderPass;
VkPipelineLayout g_vulkanPipelineLayout;
VkPipeline g_vulkanGraphicsPipeline;
VkImage g_vulkanDepthImage;
VkDeviceMemory g_vulkanDepthImageMemory;
VkImageView g_vulkanDepthImageView;
VkFramebuffer *g_vulkanFramebuffers;
VkCommandBuffer *g_vulkanCommandBuffers;
VkBuffer *g_vulkanUniformBuffers;
VkDeviceMemory *g_vulkanUniformBuffersMemory;
// sync objects
VkSemaphore *g_vulkanImageAvailableSemaphores;
VkSemaphore *g_vulkanRenderFinishedSemaphores;
uint32_t g_vulkanMaxFramesInFlight = 2;
uint32_t g_vulkanCurrentFrame = 0;
VkFence *g_vulkanInFlightFences;
VkFence *g_vulkanImagesInFlight;



void
createRenderer()
{
    createVulkanStatics();
    createVulkanDynamics();
}

void
destroyRenderer()
{
    destroyVulkanDynamics();
    vkDestroyCommandPool(g_vulkanDevice, g_vulkanCommandPool, NULL);
    vkDestroyDevice(g_vulkanDevice, NULL);
    vkDestroySurfaceKHR(g_vulkanInstance, g_vulkanSurface, NULL);
    PFN_vkDestroyDebugUtilsMessengerEXT func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(g_vulkanInstance, "vkDestroyDebugUtilsMessengerEXT");
    if (func)
    {
        func(g_vulkanInstance, g_vulkanDebugMessenger, NULL);
    }
    vkDestroyInstance(g_vulkanInstance, NULL);
}

void destroyVulkanDynamics()
{
    // cleanup uniform buffers
    for (size_t i = 0; i < g_vulkanImageCount; i++)
    {
        vkDestroyBuffer(g_vulkanDevice, g_vulkanUniformBuffers[i], NULL);
        vkFreeMemory(g_vulkanDevice, g_vulkanUniformBuffersMemory[i], NULL);
    }

    // cleanup sync objects
    for (size_t i = 0; i < g_vulkanImageCount; i++)
    {
        vkDestroySemaphore(g_vulkanDevice, g_vulkanImageAvailableSemaphores[i], NULL);
        vkDestroySemaphore(g_vulkanDevice, g_vulkanRenderFinishedSemaphores[i], NULL);
        vkDestroyFence(g_vulkanDevice, g_vulkanInFlightFences[i], NULL);
    }

    vkFreeCommandBuffers(g_vulkanDevice, g_vulkanCommandPool, g_vulkanImageCount, g_vulkanCommandBuffers);

    vkDestroyPipelineLayout(g_vulkanDevice, g_vulkanPipelineLayout, NULL);
    vkDestroyPipeline(g_vulkanDevice, g_vulkanGraphicsPipeline, NULL);
    vkDestroyRenderPass(g_vulkanDevice, g_vulkanRenderPass, NULL);
    vkDestroySampler(g_vulkanDevice, g_vulkanTextureSampler, NULL);
    vkDestroyDescriptorSetLayout(g_vulkanDevice, g_vulkanTextureLayout, NULL);
    vkDestroyDescriptorSetLayout(g_vulkanDevice, g_vulkanUniformBufferLayout, NULL);
    vkDestroyDescriptorPool(g_vulkanDevice, g_vulkanDescriptorPool, NULL);

    vkDestroyImageView(g_vulkanDevice, g_vulkanDepthImageView, NULL);
    vkDestroyImage(g_vulkanDevice, g_vulkanDepthImage, NULL);
    vkFreeMemory(g_vulkanDevice, g_vulkanDepthImageMemory, NULL);

    /*vkDestroyImageView(g_vulkanDevice, colorImageView, NULL);
    vkDestroyImage(g_vulkanDevice, colorImage, NULL);
    vkFreeMemory(g_vulkanDevice, colorImageMemory, NULL);*/

// Assuming swapChainFramebuffers is an array of VkFramebuffer with a known size swapChainFramebufferCount
    for (size_t i = 0; i < g_vulkanImageCount; ++i) {
        vkDestroyFramebuffer(g_vulkanDevice, g_vulkanFramebuffers[i], NULL);
    }

// Assuming swapChainImageViews is an array of VkImageView with a known size swapChainImageViewCount
    for (size_t i = 0; i < g_vulkanImageCount; ++i) {
        vkDestroyImageView(g_vulkanDevice, g_vulkanImageViews[i], NULL);
    }
    vkDestroySwapchainKHR(g_vulkanDevice, g_vulkanSwapChain, NULL);
}

// vulkan debug callback
static VKAPI_ATTR VkBool32 VKAPI_CALL
vulkanDebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                    VkDebugUtilsMessageTypeFlagsEXT messageType,
                    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                    void* pUserData)
{
    // Customize the message severity handling as needed
    if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
    {
        fprintf(stderr, "validation layer: %s\n", pCallbackData->pMessage);
    }

    return VK_FALSE; // VK_FALSE indicates that the application should not be aborted
}


void
createVulkanStatics()
{
    // check for validation layer support
    if (isValidationEnabled)
    {
        uint32_t layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, NULL);

        VkLayerProperties* availableLayers = malloc(sizeof(VkLayerProperties) * layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers);

        for (size_t i = 0; i < g_vulkanValidationLayerCount; i++)
        {
            bool wasLayerFound = false;

            for (uint32_t j = 0; j < layerCount; j++)
            {
                if (strcmp(g_vulkanValidationLayers[i], availableLayers[j].layerName) == 0)
                {
                    wasLayerFound = true;
                    break;
                }
            }

            if (!wasLayerFound)
            {
                free(availableLayers); // Don't forget to free the allocated memory
                printf("Validation layers requested, but not available!\n");
                exit(1);
            }
        }

        free(availableLayers); // Don't forget to free the allocated memory
    }

    // optional app info
    VkApplicationInfo appInfo   = {};
    appInfo.sType				= VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName	= "Sky Lion Game";
    appInfo.applicationVersion	= VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName			= "Sky Lion Engine";
    appInfo.engineVersion		= VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion			= VK_API_VERSION_1_0;

    // instance creation info
    VkInstanceCreateInfo createInfo = {};
    createInfo.sType			    = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo     = &appInfo;

    // required extensions for SDL & validation layers if enabled
    Uint32 finalExtensionCount;
    Uint32 sdlExtensionCount = 0;
    char const* const* sdlExtensions = SDL_Vulkan_GetInstanceExtensions(&sdlExtensionCount);
    finalExtensionCount = sdlExtensionCount + (isValidationEnabled ? 1 : 0);
    const char** finalExtensions = malloc(sizeof(char*) * (finalExtensionCount));
    for (Uint32 i = 0; i < sdlExtensionCount; i++)
    {
        finalExtensions[i] = sdlExtensions[i];
    }
    if (isValidationEnabled)
    {
        finalExtensions[sdlExtensionCount] = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;
    }
    createInfo.enabledExtensionCount	= finalExtensionCount;
    createInfo.ppEnabledExtensionNames	= finalExtensions;

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = {};
    if (isValidationEnabled)
    {
        createInfo.enabledLayerCount	= g_vulkanValidationLayerCount;
        createInfo.ppEnabledLayerNames	= g_vulkanValidationLayers;

        debugCreateInfo.sType           = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        debugCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
                                        | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
                                        | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        debugCreateInfo.messageType     = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
                                        | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
                                        | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        debugCreateInfo.pfnUserCallback = vulkanDebugCallback;
        debugCreateInfo.pUserData       = NULL; // Optional

        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
    }
    else
    {
        createInfo.enabledLayerCount	= 0;
        createInfo.pNext				= NULL;
    }

    if (vkCreateInstance(&createInfo, NULL, &g_vulkanInstance) != VK_SUCCESS)
    {
        printf("Failed to create vulkan instance!\n");
        exit(1);
    }
    // instance creation cleanup
    free(finalExtensions); // Don't forget to free the allocated memory

    // debug messenger creation
    if (isValidationEnabled)
    {
        PFN_vkCreateDebugUtilsMessengerEXT func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(g_vulkanInstance, "vkCreateDebugUtilsMessengerEXT");
        if (func)
        {
            if (func(g_vulkanInstance, &debugCreateInfo, NULL, &g_vulkanDebugMessenger) != VK_SUCCESS)
            {
                printf("Failed to set up debug messenger!\n");
                exit(1);
            }
        }
    }

    // surface creation
    if (!SDL_Vulkan_CreateSurface(getWindow()->window, g_vulkanInstance, NULL, &g_vulkanSurface))
    {
        printf("Failed to create window surface!\n");
        exit(1);
    }
    else
    {
        printf("Window surface created successfully!\n");
    }

    uint32_t deviceCount = { 0 };
    vkEnumeratePhysicalDevices(g_vulkanInstance, &deviceCount, NULL);

    if (deviceCount == 0)
    {
        printf("Failed to find GPUs with Vulkan support!\n");
        exit(1);
    }

    VkPhysicalDevice* devices = (VkPhysicalDevice*)malloc(sizeof(VkPhysicalDevice) * deviceCount);
    if (devices == NULL)
    {
        fprintf(stderr, "Failed to allocate memory for physical devices\n");
        exit(1); // Handle memory allocation failure more gracefully in real applications
    }

    vkEnumeratePhysicalDevices(g_vulkanInstance, &deviceCount, devices);

    for (uint32_t deviceIndex = 0; deviceIndex < deviceCount; ++deviceIndex)
    {
        uint32_t graphicsFamily;
        uint32_t presentFamily;
        bool graphicsFamilyFound = false;
        bool presentFamilyFound = false;

        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(devices[deviceIndex], &queueFamilyCount, NULL);
        VkQueueFamilyProperties* queueFamilies = malloc(sizeof(VkQueueFamilyProperties) * queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(devices[deviceIndex], &queueFamilyCount, queueFamilies);

        for (uint32_t i = 0; i < queueFamilyCount; i++)
        {
            if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                graphicsFamily = i;
                graphicsFamilyFound = true;
            }

            VkBool32 presentSupport = VK_FALSE;
            vkGetPhysicalDeviceSurfaceSupportKHR(devices[deviceIndex], i, g_vulkanSurface, &presentSupport);
            if (presentSupport)
            {
                presentFamily = i;
                presentFamilyFound = true;
            }

            if (graphicsFamilyFound && presentFamilyFound)
            {
                break;
            }
        }

        if (!graphicsFamilyFound || !presentFamilyFound)
        {
            free(queueFamilies);
            continue;
        }
        free(queueFamilies);

        // 2. Check device extension support
        bool extensionsSupported = false;
        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(devices[deviceIndex], NULL, &extensionCount, NULL);
        VkExtensionProperties* availableExtensions = malloc(sizeof(VkExtensionProperties) * extensionCount);
        vkEnumerateDeviceExtensionProperties(devices[deviceIndex], NULL, &extensionCount, availableExtensions);

        int requiredExtensionsFound = 0;
        uint32_t requiredDeviceExtensionCount = 1;
        const char* requiredDeviceExtensions[] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
        for (uint32_t i = 0; i < requiredDeviceExtensionCount; i++)
        {
            for (uint32_t j = 0; j < extensionCount; j++)
            {
                if (strcmp(requiredDeviceExtensions[i], availableExtensions[j].extensionName) == 0) {
                    requiredExtensionsFound++;
                    break;
                }
            }
        }
        if (requiredExtensionsFound == requiredDeviceExtensionCount)
        {
            extensionsSupported = true;
        }
        free(availableExtensions);
        if (!extensionsSupported)
        {
            continue;
        }

        VkSurfaceFormatKHR* surfaceFormats;
        VkPresentModeKHR* presentModes;
        VkSurfaceCapabilitiesKHR surfaceCapabilities;

        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(devices[deviceIndex], g_vulkanSurface, &surfaceCapabilities);

        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(devices[deviceIndex], g_vulkanSurface, &formatCount, NULL);

        if (formatCount != 0)
        {
            surfaceFormats = malloc(sizeof(VkSurfaceFormatKHR) * formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(devices[deviceIndex], g_vulkanSurface, &formatCount, surfaceFormats);
        }

        uint32_t presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(devices[deviceIndex], g_vulkanSurface, &presentModeCount, NULL);

        if (presentModeCount != 0)
        {
            presentModes = malloc(sizeof(VkPresentModeKHR) * presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(devices[deviceIndex], g_vulkanSurface, &presentModeCount, presentModes);
        }

        if (surfaceFormats == NULL && presentModes == NULL)
        {
            continue;
        }

        // 4. Check physical device features
        VkPhysicalDeviceFeatures supportedFeatures;
        vkGetPhysicalDeviceFeatures(devices[deviceIndex], &supportedFeatures);

        if (!supportedFeatures.samplerAnisotropy)
        {
            continue;
        }

        g_vulkanPhysicalDevice      = devices[deviceIndex];
        g_vulkanGraphicsFamily      = graphicsFamily;
        g_vulkanPresentFamily       = presentFamily;
        g_vulkanSurfaceFormats      = surfaceFormats;
        g_vulkanPresentModes        = presentModes;
        g_vulkanSurfaceCapabilities = surfaceCapabilities;
        g_vulkanSurfaceFormatCount  = formatCount;
        g_vulkanPresentModeCount    = presentModeCount;

        vkGetPhysicalDeviceProperties(g_vulkanPhysicalDevice, &g_vulkanPhysicalDeviceProperties);
        vkGetPhysicalDeviceMemoryProperties(g_vulkanPhysicalDevice, &g_vulkanPhysicalDeviceMemoryProperties);
        vkGetPhysicalDeviceFeatures(g_vulkanPhysicalDevice, &g_vulkanDeviceFeatures);
        break;
    }

    free(devices); // Don't forget to free the allocated memory

    if (g_vulkanPhysicalDevice == VK_NULL_HANDLE)
    {
        fprintf(stderr, "Failed to find a suitable GPU!\n");
        exit(1); // Consider handling this error more gracefully
    }

    printf("Physical device found!\n");


    // Unique queue families: in C, we just use a simple array with a check to avoid duplicates
    uint32_t uniqueQueueFamilies[2] = {g_vulkanGraphicsFamily, g_vulkanPresentFamily};
    uint32_t uniqueQueueFamilyCount = (g_vulkanGraphicsFamily != g_vulkanPresentFamily) ? 2 : 1;

    VkDeviceQueueCreateInfo* queueCreateInfos = (VkDeviceQueueCreateInfo*)malloc(sizeof(VkDeviceQueueCreateInfo) * uniqueQueueFamilyCount);
    float queuePriority = 1.0f;

    for (uint32_t i = 0; i < uniqueQueueFamilyCount; ++i)
    {
        VkDeviceQueueCreateInfo queueCreateInfo = {0};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = uniqueQueueFamilies[i];
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos[i] = queueCreateInfo;
    }

    VkPhysicalDeviceFeatures deviceFeatures = {0};
    deviceFeatures.samplerAnisotropy = VK_TRUE;

    // Replace physicalDevice->getDeviceExtensions() and instance->getValidationLayers()
    // with your actual arrays of extensions and validation layers
    const char* deviceExtensions[] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
    uint32_t deviceExtensionCount = 1;

    const char* validationLayers[] = {"VK_LAYER_KHRONOS_validation"};
    uint32_t validationLayerCount = 1;

    VkDeviceCreateInfo deviceCreateInfo = {0};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.queueCreateInfoCount = uniqueQueueFamilyCount;
    deviceCreateInfo.pQueueCreateInfos = queueCreateInfos;
    deviceCreateInfo.pEnabledFeatures = &deviceFeatures;
    deviceCreateInfo.enabledExtensionCount = deviceExtensionCount;
    deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions;

    if (isValidationEnabled)
    {
        deviceCreateInfo.enabledLayerCount = validationLayerCount;
        deviceCreateInfo.ppEnabledLayerNames = validationLayers;
    }
    else
    {
        deviceCreateInfo.enabledLayerCount = 0;
    }

    if (vkCreateDevice(g_vulkanPhysicalDevice, &deviceCreateInfo, NULL, &g_vulkanDevice) != VK_SUCCESS)
    {
        fprintf(stderr, "Failed to create logical device!\n");
        exit(1); // Consider handling this error more gracefully
    }

    printf("Logical device created!\n");

    // create queues
    vkGetDeviceQueue(g_vulkanDevice, g_vulkanGraphicsFamily, 0, &g_vulkanGraphicsQueue);
    vkGetDeviceQueue(g_vulkanDevice, g_vulkanPresentFamily, 0, &g_vulkanPresentQueue);

    // Cleanup
    free(queueCreateInfos);

    // create command pool
    VkCommandPoolCreateInfo poolInfo = {};
    poolInfo.sType				= VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex	= g_vulkanGraphicsFamily;
    poolInfo.flags				= VK_COMMAND_POOL_CREATE_TRANSIENT_BIT
        | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    if (vkCreateCommandPool(g_vulkanDevice, &poolInfo, NULL, &g_vulkanCommandPool) != VK_SUCCESS)
    {
        fprintf(stderr, "Failed to create command pool!\n");
        exit(1); // Consider handling this error more gracefully
    }
}

void
createVulkanDynamics()
{
    VkSurfaceFormatKHR surfaceFormat;
    for (uint32_t i = 0; i < g_vulkanSurfaceFormatCount; i++)
    {
        if (g_vulkanSurfaceFormats[i].format == VK_FORMAT_B8G8R8A8_SRGB && g_vulkanSurfaceFormats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            surfaceFormat = g_vulkanSurfaceFormats[i];
        }
        else
        {
            surfaceFormat = g_vulkanSurfaceFormats[0];

        }
    }

    VkPresentModeKHR presentMode;
    for (uint32_t i = 0; i < g_vulkanPresentModeCount; i++)
    {
        if (g_vulkanPresentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
            // Preferred mode: Triple buffering
            presentMode = g_vulkanPresentModes[i];
        }
        else
        {
            // Fallback mode: Double buffering
            presentMode = VK_PRESENT_MODE_FIFO_KHR;
        }
    }

    VkExtent2D extent;
    if (g_vulkanSurfaceCapabilities.currentExtent.width != UINT32_MAX)
    {
        extent = g_vulkanSurfaceCapabilities.currentExtent;
    }
    else
    {
        VkExtent2D actualExtent;
        actualExtent.width = SDL_max(g_vulkanSurfaceCapabilities.minImageExtent.width, SDL_min(g_vulkanSurfaceCapabilities.maxImageExtent.width, actualExtent.width));
        actualExtent.height = SDL_max(g_vulkanSurfaceCapabilities.minImageExtent.height, SDL_min(g_vulkanSurfaceCapabilities.maxImageExtent.height, actualExtent.height));
        extent = actualExtent;
    }

    g_vulkanImageCount = g_vulkanSurfaceCapabilities.minImageCount + 1;

    if ((g_vulkanSurfaceCapabilities.maxImageCount > 0)
        && (g_vulkanImageCount > g_vulkanSurfaceCapabilities.maxImageCount))
    {
        g_vulkanImageCount = g_vulkanSurfaceCapabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR swapChainCreateInfo = {};
    swapChainCreateInfo.sType			= VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapChainCreateInfo.surface			= g_vulkanSurface;
    swapChainCreateInfo.minImageCount	= g_vulkanImageCount;
    swapChainCreateInfo.imageFormat		= surfaceFormat.format;
    swapChainCreateInfo.imageColorSpace	= surfaceFormat.colorSpace;
    swapChainCreateInfo.imageExtent		= extent;
    swapChainCreateInfo.imageArrayLayers = 1;
    swapChainCreateInfo.imageUsage		= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    
    uint32_t queueFamilyIndices[] = { g_vulkanGraphicsFamily, g_vulkanPresentFamily };

    if (g_vulkanGraphicsFamily != g_vulkanPresentFamily)
    {
        swapChainCreateInfo.imageSharingMode		= VK_SHARING_MODE_CONCURRENT;
        swapChainCreateInfo.queueFamilyIndexCount	= 2;
        swapChainCreateInfo.pQueueFamilyIndices		= queueFamilyIndices;
    }
    else
    {
        swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }

    swapChainCreateInfo.preTransform	= g_vulkanSurfaceCapabilities.currentTransform;
    swapChainCreateInfo.compositeAlpha	= VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapChainCreateInfo.presentMode		= presentMode;
    swapChainCreateInfo.clipped			= VK_TRUE;
    swapChainCreateInfo.oldSwapchain	= VK_NULL_HANDLE;

    if (vkCreateSwapchainKHR(g_vulkanDevice, &swapChainCreateInfo, NULL, &g_vulkanSwapChain) != VK_SUCCESS)
    {
        fprintf(stderr, "Failed to create swap chain!\n");
        exit(1); // Consider handling this error more gracefully
    }

    vkGetSwapchainImagesKHR(g_vulkanDevice, g_vulkanSwapChain, &g_vulkanImageCount, NULL);
    g_vulkanImages = malloc(sizeof(VkImage) * g_vulkanImageCount);
    vkGetSwapchainImagesKHR(g_vulkanDevice, g_vulkanSwapChain, &g_vulkanImageCount, g_vulkanImages);

    g_vulkanImageFormat = surfaceFormat.format;
    g_vulkanSwapExtent = extent;

    printf("Swap chain created!\n");

    // image views

    g_vulkanImageViews = malloc(sizeof(VkImageView) * g_vulkanImageCount);

    for (size_t imageIndex = 0 ; imageIndex < g_vulkanImageCount; ++imageIndex)
    {
        // TODO: abstract to another function since it's used by textures as well
        VkImageViewCreateInfo createInfo = {};
        createInfo.sType							= VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image							= g_vulkanImages[imageIndex];
        createInfo.viewType							= VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format							= g_vulkanImageFormat;
        createInfo.components.r						= VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g						= VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b						= VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a						= VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.subresourceRange.aspectMask		= VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel	= 0;
        createInfo.subresourceRange.levelCount		= 1;
        createInfo.subresourceRange.baseArrayLayer	= 0;
        createInfo.subresourceRange.layerCount		= 1;

        if (vkCreateImageView(g_vulkanDevice, &createInfo, NULL, &g_vulkanImageViews[imageIndex]) != VK_SUCCESS)
        {
            fprintf(stderr, "Failed to create image views!\n");
            exit(1); // Consider handling this error more gracefully
        }
    }

    printf("Image views created!\n");

    // descriptor pool

    // all descriptors used by the shader
    uint32_t descriptorPoolSizeCount = 2;
    VkDescriptorPoolSize *poolSizes = malloc(sizeof(VkDescriptorPoolSize) * descriptorPoolSizeCount);
    poolSizes[0].type				= VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[0].descriptorCount	= 100;
    poolSizes[1].type				= VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSizes[1].descriptorCount	= 100;

    VkDescriptorPoolCreateInfo poolInfo = {};
    poolInfo.sType			            = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount	            = descriptorPoolSizeCount;
    poolInfo.pPoolSizes		            = poolSizes;
    poolInfo.maxSets		            = 100;

    if (vkCreateDescriptorPool(g_vulkanDevice, &poolInfo, NULL, &g_vulkanDescriptorPool) != VK_SUCCESS)
    {
        fprintf(stderr, "Failed to create descriptor pool!\n");
        exit(1); // Consider handling this error more gracefully
    }

    free(poolSizes);

    printf("Descriptor pool created!\n");

    // descriptor set layouts
    // uniform buffer
    VkDescriptorSetLayoutBinding uboLayoutBinding   = {};
    uboLayoutBinding.binding                        = 0;
    uboLayoutBinding.descriptorType                 = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uboLayoutBinding.descriptorCount                = 1;
    uboLayoutBinding.stageFlags                     = VK_SHADER_STAGE_VERTEX_BIT;
    uboLayoutBinding.pImmutableSamplers             = NULL;

    VkDescriptorSetLayoutCreateInfo uboLayoutInfo   = {};
    uboLayoutInfo.sType                             = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    uboLayoutInfo.bindingCount                      = 1;
    uboLayoutInfo.pBindings                         = &uboLayoutBinding;

    if (vkCreateDescriptorSetLayout(g_vulkanDevice, &uboLayoutInfo, NULL, &g_vulkanUniformBufferLayout) != VK_SUCCESS)
    {
        fprintf(stderr, "Failed to create descriptor set layout!\n");
        exit(1); // Consider handling this error more gracefully
    }

    // image sampler
    VkDescriptorSetLayoutBinding samplerLayoutBinding   = {};
    samplerLayoutBinding.binding                        = 0;
    samplerLayoutBinding.descriptorCount                = 1;
    samplerLayoutBinding.descriptorType                 = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    samplerLayoutBinding.pImmutableSamplers             = NULL;
    samplerLayoutBinding.stageFlags                     = VK_SHADER_STAGE_FRAGMENT_BIT;

    VkDescriptorSetLayoutCreateInfo samplerLayoutInfo   = {};
    samplerLayoutInfo.sType                             = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    samplerLayoutInfo.bindingCount                      = 1;
    samplerLayoutInfo.pBindings                         = &samplerLayoutBinding;

    if (vkCreateDescriptorSetLayout(g_vulkanDevice, &samplerLayoutInfo, NULL, &g_vulkanTextureLayout) != VK_SUCCESS)
    {
        fprintf(stderr, "Failed to create descriptor set layout!\n");
        exit(1); // Consider handling this error more gracefully
    }

    printf("Descriptor set layouts created!\n");

    // TODO: use of magic number makes code unclear 2. Is this related to pool sizes?
    g_vulkanLayouts     = malloc(sizeof(VkDescriptorSetLayout) * g_vulkanLayoutsCount);
    g_vulkanLayouts[0]  = g_vulkanUniformBufferLayout;
    g_vulkanLayouts[1]  = g_vulkanTextureLayout;

    // texture sampler
    VkSamplerCreateInfo samplerInfo     = {};
    samplerInfo.sType                   = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter               = VK_FILTER_LINEAR;	// over sampling
    samplerInfo.minFilter               = VK_FILTER_LINEAR;	// under sampling
    samplerInfo.addressModeU            = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV            = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW            = VK_SAMPLER_ADDRESS_MODE_REPEAT;;
    samplerInfo.anisotropyEnable        = VK_TRUE;
    samplerInfo.maxAnisotropy           = g_vulkanPhysicalDeviceProperties.limits.maxSamplerAnisotropy;
    samplerInfo.borderColor             = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable           = VK_FALSE;
    samplerInfo.compareOp               = VK_COMPARE_OP_ALWAYS;
    samplerInfo.mipmapMode              = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.mipLodBias              = 0.0f;
    samplerInfo.minLod                  = 0.0f;
    samplerInfo.maxLod                  = 0.0f;

    if (vkCreateSampler(g_vulkanDevice, &samplerInfo, NULL, &g_vulkanTextureSampler) != VK_SUCCESS)
    {
        fprintf(stderr, "Failed to create texture sampler!\n");
        exit(1); // Consider handling this error more gracefully
    }

    printf("Texture sampler created!\n");

    // render pass

    VkAttachmentDescription colorAttachment = {};
    colorAttachment.format			= g_vulkanImageFormat;
    colorAttachment.samples			= VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp			= VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp			= VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp	= VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp	= VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout	= VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout		= VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef = {};
    colorAttachmentRef.attachment	= 0;
    colorAttachmentRef.layout		= VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkFormat depthFormats[] = { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT };
    size_t depthFormatCount = sizeof(depthFormats) / sizeof(depthFormats[0]);
    VkImageTiling tiling = VK_IMAGE_TILING_OPTIMAL;
    VkFormatFeatureFlags features = VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT;

    g_vulkanDepthFormat = VK_FORMAT_UNDEFINED;
    for (size_t i = 0; i < depthFormatCount; ++i) {
        VkFormatProperties properties;
        vkGetPhysicalDeviceFormatProperties(g_vulkanPhysicalDevice, depthFormats[i], &properties);

        if ((tiling == VK_IMAGE_TILING_LINEAR && (properties.linearTilingFeatures & features) == features)
        || (tiling == VK_IMAGE_TILING_OPTIMAL && (properties.optimalTilingFeatures & features) == features))
        {
            g_vulkanDepthFormat = depthFormats[i];
            break;
        }
    }

    if (g_vulkanDepthFormat == VK_FORMAT_UNDEFINED) {
        fprintf(stderr, "Failed to find a supported format.\n");
        exit(EXIT_FAILURE); // Consider more graceful error handling in real applications
    }

    VkAttachmentDescription depthAttachment = {};
    depthAttachment.format			= g_vulkanDepthFormat;
    depthAttachment.samples			= VK_SAMPLE_COUNT_1_BIT;
    depthAttachment.loadOp			= VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp			= VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.stencilLoadOp	= VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachment.stencilStoreOp	= VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.initialLayout	= VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment.finalLayout		= VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthAttachmentRef = {};
    depthAttachmentRef.attachment	= 1;
    depthAttachmentRef.layout		= VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint		= VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount	= 1;
    subpass.pColorAttachments		= &colorAttachmentRef;
    subpass.pDepthStencilAttachment = &depthAttachmentRef;

    VkSubpassDependency dependency = {};
    dependency.srcSubpass			= VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass			= 0;
    dependency.srcStageMask			= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
        | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.srcAccessMask		= 0;
    dependency.dstStageMask			= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
        | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.dstAccessMask		= VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT
        | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    VkAttachmentDescription attachments[] = { colorAttachment, depthAttachment };
    uint32_t attachmentCount = 2;

    VkRenderPassCreateInfo renderPassInfo = {};
    renderPassInfo.sType			= VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount	= attachmentCount;
    renderPassInfo.pAttachments		= attachments;
    renderPassInfo.subpassCount		= 1;
    renderPassInfo.pSubpasses		= &subpass;
    renderPassInfo.dependencyCount	= 1;
    renderPassInfo.pDependencies	= &dependency;

    if (vkCreateRenderPass(g_vulkanDevice, &renderPassInfo, NULL, &g_vulkanRenderPass) != VK_SUCCESS)
    {
        fprintf(stderr, "Failed to create render pass!\n");
        exit(1); // Consider handling this error more gracefully
    }

    printf("Render pass created!\n");

    // create graphics pipeline

    const char* shaderFilenames[] = {"shaders/shader.vert.spv", "shaders/shader.frag.spv"};

    uint32_t shaderModuleCount = 2;
    VkShaderModule shaderModules[2];

    for (int shaderIndex = 0; shaderIndex < 2; ++shaderIndex) {
        FILE* file = fopen(shaderFilenames[shaderIndex], "rb");
        if (!file) {
            fprintf(stderr, "Failed to open file %s\n", shaderFilenames[shaderIndex]);
            exit(EXIT_FAILURE);
        }

        fseek(file, 0, SEEK_END);
        size_t fileSize = ftell(file);
        rewind(file);

        // Allocate memory for the file content with proper alignment
        uint32_t* buffer = malloc(fileSize);
        if (!buffer) {
            fprintf(stderr, "Failed to allocate memory for shader file %s\n", shaderFilenames[shaderIndex]);
            fclose(file);
            exit(EXIT_FAILURE);
        }

        fread(buffer, 1, fileSize, file);
        fclose(file);

        VkShaderModuleCreateInfo shaderCreateInfo = {};
        shaderCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        shaderCreateInfo.codeSize = fileSize;
        shaderCreateInfo.pCode = buffer;

        VkResult shaderCreateResult = vkCreateShaderModule(g_vulkanDevice, &shaderCreateInfo, NULL, &shaderModules[shaderIndex]);
        if (shaderCreateResult != VK_SUCCESS) {  // Corrected condition
            fprintf(stderr, "Failed to create shader module for %s\n", shaderFilenames[shaderIndex]);
            free(buffer);  // Free the buffer to avoid memory leak
            exit(EXIT_FAILURE);
        }

        free(buffer);  // Free the buffer after use to avoid memory leak
    }

    // vertex shader info
    VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
    vertShaderStageInfo.sType	= VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage	= VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module	= shaderModules[0]; // vertex shader
    vertShaderStageInfo.pName	= "main";

    // fragment shader info
    VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
    fragShaderStageInfo.sType	= VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage	= VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module	= shaderModules[1]; // fragment shader
    fragShaderStageInfo.pName	= "main";

    // info for all shader stages
    VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

    VkVertexInputBindingDescription bindingDescription = {};
    bindingDescription.binding		= 0;
    bindingDescription.stride		= sizeof(Vertex);
    bindingDescription.inputRate	= VK_VERTEX_INPUT_RATE_VERTEX;

    VkVertexInputAttributeDescription attributeDescriptions[6];
    attributeDescriptions[0].binding = 0;
    attributeDescriptions[0].location = 0;
    attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[0].offset = offsetof(Vertex, pos);
    // Normal
    attributeDescriptions[1].binding = 0;
    attributeDescriptions[1].location = 1;
    attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[1].offset = offsetof(Vertex, normal);
    // UV
    attributeDescriptions[2].binding = 0;
    attributeDescriptions[2].location = 2;
    attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[2].offset = offsetof(Vertex, uv);
    // Color
    attributeDescriptions[3].binding = 0;
    attributeDescriptions[3].location = 3;
    attributeDescriptions[3].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[3].offset = offsetof(Vertex, color);
    // Joint indices
    attributeDescriptions[4].binding = 0;
    attributeDescriptions[4].location = 4;
    attributeDescriptions[4].format = VK_FORMAT_R32G32B32A32_SFLOAT;
    attributeDescriptions[4].offset = offsetof(Vertex, jointIndices);
    // Joint weights
    attributeDescriptions[5].binding = 0;
    attributeDescriptions[5].location = 5;
    attributeDescriptions[5].format = VK_FORMAT_R32G32B32A32_SFLOAT;
    attributeDescriptions[5].offset = offsetof(Vertex, jointWeights);

    // info for how vertex data is loaded into the vertex shader
    VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
    vertexInputInfo.sType							= VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount	= 1;
    vertexInputInfo.pVertexBindingDescriptions		= &bindingDescription;
    vertexInputInfo.vertexAttributeDescriptionCount = 6;
    vertexInputInfo.pVertexAttributeDescriptions	= attributeDescriptions;

    // info on what kind of geometry will be drawn, IE: triangles, lines, etc...
    VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
    inputAssembly.sType						= VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology					= VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable	= VK_FALSE;

    // info on viewport size, depth, and offset
    VkViewport viewport = {};
    viewport.x			= 0.0f;
    viewport.y			= 0.0f;
    viewport.width		= (float)g_vulkanSwapExtent.width;
    viewport.height		= (float)g_vulkanSwapExtent.height;
    viewport.minDepth	= 0.0f;
    viewport.maxDepth	= 1.0f;

    // info about clipping
    VkRect2D scissor = {};
    scissor.offset.x = 0;
    scissor.offset.y = 0;
    scissor.extent = g_vulkanSwapExtent;

    // combined info on viewport and scissor
    VkPipelineViewportStateCreateInfo viewportState = {};
    viewportState.sType			= VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.pViewports	= &viewport;
    viewportState.scissorCount	= 1;
    viewportState.pScissors		= &scissor;

    // rasterization stage info
    VkPipelineRasterizationStateCreateInfo rasterizer = {};
    rasterizer.sType					= VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable			= VK_FALSE;
    rasterizer.rasterizerDiscardEnable	= VK_FALSE;
    rasterizer.polygonMode				= VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth				= 1.0f;
    rasterizer.cullMode					= VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace				= VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizer.depthBiasEnable			= VK_FALSE;

    // multisampling info
    VkPipelineMultisampleStateCreateInfo multisampling = {};
    multisampling.sType					= VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable	= VK_FALSE;
    multisampling.rasterizationSamples	= VK_SAMPLE_COUNT_1_BIT;

    // depth stencil
    VkPipelineDepthStencilStateCreateInfo depthStencil = {};
    depthStencil.sType					= VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencil.depthTestEnable		= VK_TRUE;
    depthStencil.depthWriteEnable		= VK_TRUE;
    depthStencil.depthCompareOp			= VK_COMPARE_OP_LESS;
    depthStencil.depthBoundsTestEnable	= VK_FALSE;
    depthStencil.minDepthBounds			= 0.0f; // optional
    depthStencil.maxDepthBounds			= 1.0f; // optional
    depthStencil.stencilTestEnable		= VK_FALSE;

    // color blending info
    VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT
        | VK_COLOR_COMPONENT_G_BIT
        | VK_COLOR_COMPONENT_B_BIT
        | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable	= VK_FALSE;

    VkPipelineColorBlendStateCreateInfo colorBlending = {};
    colorBlending.sType				= VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable		= VK_FALSE;
    colorBlending.logicOp			= VK_LOGIC_OP_COPY;
    colorBlending.attachmentCount	= 1;
    colorBlending.pAttachments		= &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f;
    colorBlending.blendConstants[1] = 0.0f;
    colorBlending.blendConstants[2] = 0.0f;
    colorBlending.blendConstants[3] = 0.0f;

    VkPushConstantRange pushConstantRange = {};
    pushConstantRange.stageFlags	= VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    pushConstantRange.offset		= 0;
    pushConstantRange.size			= sizeof(PushConstantData);

    // pipeline layout
    VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
    pipelineLayoutInfo.sType					= VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount			= g_vulkanLayoutsCount;
    pipelineLayoutInfo.pSetLayouts				= g_vulkanLayouts;
    pipelineLayoutInfo.pushConstantRangeCount	= 1;
    pipelineLayoutInfo.pPushConstantRanges		= &pushConstantRange;

    if (vkCreatePipelineLayout(g_vulkanDevice, &pipelineLayoutInfo, NULL, &g_vulkanPipelineLayout) != VK_SUCCESS)
    {
        fprintf(stderr, "Failed to create pipeline layout!\n");
        exit(1); // Consider handling this error more gracefully
    }

    printf("Pipeline layout created!\n");

    // Pipeline creation info
    VkGraphicsPipelineCreateInfo pipelineInfo = {};
    pipelineInfo.sType					= VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount				= 2;
    pipelineInfo.pStages				= shaderStages;
    pipelineInfo.pVertexInputState		= &vertexInputInfo;
    pipelineInfo.pInputAssemblyState	= &inputAssembly;
    pipelineInfo.pViewportState			= &viewportState;
    pipelineInfo.pRasterizationState	= &rasterizer;
    pipelineInfo.pMultisampleState		= &multisampling;
    pipelineInfo.pDepthStencilState		= &depthStencil;
    pipelineInfo.pColorBlendState		= &colorBlending;
    pipelineInfo.pDynamicState			= NULL;
    pipelineInfo.layout					= g_vulkanPipelineLayout;
    pipelineInfo.renderPass				= g_vulkanRenderPass;
    pipelineInfo.subpass				= 0;
    pipelineInfo.basePipelineHandle		= VK_NULL_HANDLE;
    pipelineInfo.basePipelineIndex		= -1;

    if (vkCreateGraphicsPipelines(g_vulkanDevice, VK_NULL_HANDLE, 1, &pipelineInfo, NULL, &g_vulkanGraphicsPipeline) != VK_SUCCESS)
    {
        fprintf(stderr, "Failed to create graphics pipeline!\n");
        exit(1); // Consider handling this error more gracefully
    }

    printf("Graphics pipeline created!\n");

    // shaders are loaded and the modules are no longer required
    for (int shaderIndex = 0; shaderIndex < 2; ++shaderIndex)
    {
        vkDestroyShaderModule(g_vulkanDevice, shaderModules[shaderIndex], NULL);
    }

    // Image creation info
    VkImageCreateInfo imageInfo = {};
    imageInfo.sType			= VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType		= VK_IMAGE_TYPE_2D;
    imageInfo.extent.width	= g_vulkanSwapExtent.width;
    imageInfo.extent.height = g_vulkanSwapExtent.height;
    imageInfo.extent.depth	= 1;
    imageInfo.mipLevels		= 1;
    imageInfo.arrayLayers	= 1;
    imageInfo.format		= g_vulkanDepthFormat;
    imageInfo.tiling		= VK_IMAGE_TILING_OPTIMAL;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage			= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    imageInfo.sharingMode	= VK_SHARING_MODE_EXCLUSIVE;
    imageInfo.samples		= VK_SAMPLE_COUNT_1_BIT;
    imageInfo.flags			= 0;

    if (vkCreateImage(g_vulkanDevice, &imageInfo, NULL, &g_vulkanDepthImage) != VK_SUCCESS)
    {
        fprintf(stderr, "Failed to create image!\n");
        exit(1); // Consider handling this error more gracefully
    }

    printf("Depth image created!\n");

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(g_vulkanDevice, g_vulkanDepthImage, &memRequirements);

    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType				= VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize	= memRequirements.size;
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(g_vulkanPhysicalDevice, &memProperties);

    uint32_t memoryTypeIndex = 0;
    bool memIndexFound = false;
    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
    {
        if ( (memRequirements.memoryTypeBits & (1 << i)) && ((memProperties.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) == VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) )
        {
            memoryTypeIndex = i;
            memIndexFound = true;
        }
    }
    if (!memIndexFound)
    {
        fprintf(stderr, "Failed to find suitable memory type!\n");
        exit(1); // Consider handling this error more gracefully
    }

    allocInfo.memoryTypeIndex = memoryTypeIndex;

    if (vkAllocateMemory(g_vulkanDevice, &allocInfo, NULL, &g_vulkanDepthImageMemory) != VK_SUCCESS)
    {
        fprintf(stderr, "Failed to allocate image memory!\n");
        exit(1); // Consider handling this error more gracefully
    }

    vkBindImageMemory(g_vulkanDevice, g_vulkanDepthImage, g_vulkanDepthImageMemory, 0);

    printf("Depth image memory allocated");

    VkImageViewCreateInfo depthViewInfo = {};
    depthViewInfo.sType								= VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    depthViewInfo.image								= g_vulkanDepthImage;
    depthViewInfo.viewType							= VK_IMAGE_VIEW_TYPE_2D;
    depthViewInfo.format							= g_vulkanDepthFormat;
    depthViewInfo.subresourceRange.aspectMask		= VK_IMAGE_ASPECT_DEPTH_BIT;
    depthViewInfo.subresourceRange.baseMipLevel		= 0;
    depthViewInfo.subresourceRange.levelCount		= 1;
    depthViewInfo.subresourceRange.baseArrayLayer	= 0;
    depthViewInfo.subresourceRange.layerCount		= 1;

    if (vkCreateImageView(g_vulkanDevice, &depthViewInfo, NULL, &g_vulkanDepthImageView) != VK_SUCCESS)
    {
        fprintf(stderr, "Failed to create image view!\n");
        exit(1); // Consider handling this error more gracefully
    }

    printf("Depth image view created!\n");

    g_vulkanFramebuffers = malloc(sizeof(VkFramebuffer) * g_vulkanImageCount);

    for (size_t i = 0; i < g_vulkanImageCount; i++)
    {

        VkImageView imageViewAttachments[2]; // C array to hold attachments
        imageViewAttachments[0] = g_vulkanImageViews[i]; // Assuming 'i' is defined in some loop or context
        imageViewAttachments[1] = g_vulkanDepthImageView;

        VkFramebufferCreateInfo framebufferInfo = {0}; // Zero initialization for a C struct
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = g_vulkanRenderPass;
        framebufferInfo.attachmentCount = 2; // Size of the attachments array
        framebufferInfo.pAttachments = imageViewAttachments; // Directly use the array
        framebufferInfo.width = g_vulkanSwapExtent.width;
        framebufferInfo.height = g_vulkanSwapExtent.height;
        framebufferInfo.layers = 1;

        VkResult result = vkCreateFramebuffer(g_vulkanDevice, &framebufferInfo, NULL, &g_vulkanFramebuffers[i]);
        if (result != VK_SUCCESS)
        {
            fprintf(stderr, "Failed to create framebuffer!\n");
            exit(EXIT_FAILURE); // Consider more graceful error handling in real applications
        }
    }

    printf("Framebuffers created!\n");

    // command buffers

    g_vulkanCommandBuffers = malloc(sizeof(VkCommandBuffer) * g_vulkanImageCount);

    VkCommandBufferAllocateInfo commandBufferAllocInfo = {};
    commandBufferAllocInfo.sType					= VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    commandBufferAllocInfo.commandPool			= g_vulkanCommandPool;
    commandBufferAllocInfo.level					= VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    commandBufferAllocInfo.commandBufferCount	= g_vulkanImageCount;

    if (vkAllocateCommandBuffers(g_vulkanDevice, &commandBufferAllocInfo, g_vulkanCommandBuffers) != VK_SUCCESS)
    {
        fprintf(stderr, "Failed to allocate command buffers!\n");
        exit(1); // Consider handling this error more gracefully
    }

    printf("Command buffers allocated!\n");

    // sync objects

    g_vulkanImageAvailableSemaphores = malloc(sizeof(VkSemaphore) * g_vulkanImageCount);
    g_vulkanRenderFinishedSemaphores = malloc(sizeof(VkSemaphore) * g_vulkanImageCount);
    g_vulkanInFlightFences = malloc(sizeof(VkFence) * g_vulkanImageCount);
    g_vulkanImagesInFlight = malloc(sizeof(VkFence) * g_vulkanImageCount);

    VkSemaphoreCreateInfo semaphoreInfo = {};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo = {};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < g_vulkanImageCount; i++)
    {
        if ((vkCreateSemaphore(g_vulkanDevice, &semaphoreInfo, NULL, &g_vulkanImageAvailableSemaphores[i]) != VK_SUCCESS)
            || (vkCreateSemaphore(g_vulkanDevice, &semaphoreInfo, NULL, &g_vulkanRenderFinishedSemaphores[i]) != VK_SUCCESS)
            || (vkCreateFence(g_vulkanDevice, &fenceInfo, NULL, &g_vulkanInFlightFences[i]) != VK_SUCCESS))
        {
            fprintf(stderr, "Failed to create semaphores or fences!\n");
            exit(1); // Consider handling this error more gracefully
        }
    }

    printf("Sync objects created!\n");

    // uniform buffers
    g_vulkanUniformBuffers = malloc(sizeof(VkBuffer) * g_vulkanImageCount);
    g_vulkanUniformBuffersMemory = malloc(sizeof(VkDeviceMemory) * g_vulkanImageCount);

    if (!g_vulkanUniformBuffers || !g_vulkanUniformBuffersMemory)
    {
        fprintf(stderr, "Failed to allocate memory for uniform buffers!\n");
        exit(1); // Consider more graceful error handling
    }

    for (size_t i = 0; i < g_vulkanImageCount; i++)
    {
        VkBufferCreateInfo bufferInfo = {0};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = sizeof(UniformBufferObject);
        bufferInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateBuffer(g_vulkanDevice, &bufferInfo, NULL, &g_vulkanUniformBuffers[i]) != VK_SUCCESS)
        {
            fprintf(stderr, "Failed to create uniform buffer!\n");
            // Cleanup allocated resources before exiting
            exit(1); // Consider more graceful error handling
        }

        VkMemoryRequirements uniformMemRequirements;
        vkGetBufferMemoryRequirements(g_vulkanDevice, g_vulkanUniformBuffers[i], &uniformMemRequirements);

        VkMemoryAllocateInfo uniformMemAllocInfo = {0};
        uniformMemAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        uniformMemAllocInfo.allocationSize = uniformMemRequirements.size;

        VkPhysicalDeviceMemoryProperties uniformMemProperties;
        vkGetPhysicalDeviceMemoryProperties(g_vulkanPhysicalDevice, &uniformMemProperties);

        bool uniformMemIndexFound = false;
        for (uint32_t j = 0; j < uniformMemProperties.memoryTypeCount; j++)
        {
            if ((uniformMemRequirements.memoryTypeBits & (1 << j)) && (uniformMemProperties.memoryTypes[j].propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) == VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
            {
                uniformMemAllocInfo.memoryTypeIndex = j;
                uniformMemIndexFound = true;
                break;
            }
        }

        if (!uniformMemIndexFound) {
            fprintf(stderr, "Failed to find suitable memory type!\n");
            // Cleanup allocated resources before exiting
            exit(1); // Consider more graceful error handling
        }

        if (vkAllocateMemory(g_vulkanDevice, &uniformMemAllocInfo, NULL, &g_vulkanUniformBuffersMemory[i]) != VK_SUCCESS) {
            fprintf(stderr, "Failed to allocate uniform buffer memory!\n");
            // Cleanup allocated resources before exiting
            exit(1); // Consider more graceful error handling
        }

        vkBindBufferMemory(g_vulkanDevice, g_vulkanUniformBuffers[i], g_vulkanUniformBuffersMemory[i], 0);
    }


    printf("Uniform buffers created!\n");
}
