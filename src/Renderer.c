//
// Created by jesse on 2/6/24.
//

#include "SkyLion.h"


VulkanContext g_vulkanContext;

// Function to check if all required extensions are supported by the device

bool
checkSwapChainSupport(VkPhysicalDevice device)
{
    uint32_t extensionCount = 0;
    vkEnumerateDeviceExtensionProperties(device, NULL, &extensionCount, NULL);

    VkExtensionProperties availableExtensions[extensionCount];
    vkEnumerateDeviceExtensionProperties(device, NULL, &extensionCount, availableExtensions);

    for (uint32_t i = 0; i < extensionCount; i++)
    {
        if (strcmp(VK_KHR_SWAPCHAIN_EXTENSION_NAME, availableExtensions[i].extensionName) == 0)
        {
            return true; // Swap chain extension is supported
        }
    }

    return false; // Swap chain extension is not supported
}
QueueFamilyIndices
findQueueFamilies(VulkanContext *context, VkPhysicalDevice device)
{
    QueueFamilyIndices indices = {0, 0, false, false};

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, NULL);

    VkQueueFamilyProperties queueFamilies[queueFamilyCount];
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies);

    for (uint32_t i = 0; i < queueFamilyCount; i++)
    {
        // Check for graphics support
        if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            indices.graphicsFamily = i;
            indices.graphicsFamilyFound = true;
        }

        // Check for presentation support
        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, context->surface, &presentSupport);
        if (presentSupport)
        {
            indices.presentFamily = i;
            indices.presentFamilyFound = true;
        }

        // Break out early if both families are found
        if (indices.graphicsFamilyFound && indices.presentFamilyFound)
        {
            break;
        }
    }

    return indices;
}

void
createVulkanDevice(VulkanContext *context)
{
    QueueFamilyIndices indices = findQueueFamilies(context, context->physicalDevice);

    VkDeviceQueueCreateInfo queueCreateInfo = {};
    queueCreateInfo.sType               = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex    = indices.graphicsFamily;
    queueCreateInfo.queueCount          = 1;

    float queuePriority = 1.0f;
    queueCreateInfo.pQueuePriorities = &queuePriority;

    VkPhysicalDeviceFeatures deviceFeatures = {};

    VkDeviceCreateInfo createInfo       = {};
    createInfo.sType                    = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pQueueCreateInfos        = &queueCreateInfo;
    createInfo.queueCreateInfoCount     = 1;
    createInfo.pEnabledFeatures         = &deviceFeatures;
    createInfo.enabledExtensionCount    = 0;

    if (context->enableValidationLayers)
    {
        createInfo.enabledLayerCount = context->validationLayerCount;
        createInfo.ppEnabledLayerNames = &context->validationLayers;
    }
    else
    {
        createInfo.enabledLayerCount = 0;
    }

    if (vkCreateDevice(context->physicalDevice, &createInfo, NULL, &context->device) != VK_SUCCESS)
    {
        printf("Failed to create logical device!\n");
        exit(1);
    }

    vkGetDeviceQueue(context->device, indices.graphicsFamily, 0, &context->graphicsQueue);

    printf("Logical device created.\n");
}

// Function to check if a physical device is suitable for the application's needs
bool
isDeviceSuitable(VulkanContext *context, VkPhysicalDevice device)
{
    QueueFamilyIndices indices = findQueueFamilies(context, device);
    return indices.graphicsFamilyFound && indices.presentFamilyFound && checkSwapChainSupport(device);
}

void
createPhysicalDevice(VulkanContext* context)
{
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(context->instance, &deviceCount, NULL);
    if (deviceCount == 0)
    {
        printf("Failed to find GPUs with Vulkan support.\n");
        return;
    }

    VkPhysicalDevice devices[deviceCount];
    vkEnumeratePhysicalDevices(context->instance, &deviceCount, devices);

    for (uint32_t i = 0; i < deviceCount; i++)
    {
        if (isDeviceSuitable(context, devices[i]))
        {
            context->physicalDevice = devices[i];
            break;
        }
    }

    if (context->physicalDevice == VK_NULL_HANDLE)
    {
        printf("Failed to find a suitable GPU.\n");
    }
    else
    {
        printf("Physical device selected.\n");
    }
}

void
createSurface(VulkanContext *context)
{
    if (!SDL_Vulkan_CreateSurface(SLWindow, context->instance, NULL, &context->surface))
    {
        printf("Failed to create window surface!\n");
        exit(1);
    }
    else
    {
        printf("Window surface created successfully!\n");
    }
}

const char**
getVulkanExtensions(VulkanContext *context, Uint32 *extensionCount)
{
    // Get the count and list of required SDL extensions
    Uint32 sdlExtensionCount = 0;
    char const* const* sdlExtensions = SDL_Vulkan_GetInstanceExtensions(&sdlExtensionCount);

    // Calculate total extension count
    *extensionCount = sdlExtensionCount + (context->enableValidationLayers ? 1 : 0);

    // Allocate memory for the final extensions array
    const char** finalExtensions = malloc(sizeof(char*) * (*extensionCount));
    if (!finalExtensions)
    {
        // Handle allocation failure if needed
        *extensionCount = 0; // Indicate failure
        return NULL;
    }

    // Copy SDL extensions into the final array
    for (Uint32 i = 0; i < sdlExtensionCount; i++)
    {
        finalExtensions[i] = sdlExtensions[i];
    }

    // Add the debug utils extension name if validation layers are enabled
    if (context->enableValidationLayers)
    {
        finalExtensions[sdlExtensionCount] = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;
    }

    return finalExtensions; // Caller is responsible for freeing this memory
}


static VKAPI_ATTR VkBool32 VKAPI_CALL
vulkanDebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT      messageSeverity,
                    VkDebugUtilsMessageTypeFlagsEXT             messageType,
                    const VkDebugUtilsMessengerCallbackDataEXT  *pCallbackData,
                    void                                        *pUserData)
{
    // Customize the message severity handling as needed
    if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
    {
        fprintf(stderr, "validation layer: %s\n", pCallbackData->pMessage);
    }

    return VK_FALSE; // VK_FALSE indicates that the application should not be aborted
}

VkResult
createDebugUtilsMessengerEXT(VkInstance                                 instance,
                             const VkDebugUtilsMessengerCreateInfoEXT   *pCreateInfo,
                             const VkAllocationCallbacks                *pAllocator,
                             VkDebugUtilsMessengerEXT                   *pDebugMessenger)
{
    PFN_vkCreateDebugUtilsMessengerEXT func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != NULL)
    {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    }
    else
    {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void
destroyDebugUtilsMessengerEXT(VkInstance                    instance,
                              VkDebugUtilsMessengerEXT      debugMessenger,
                              const VkAllocationCallbacks*  pAllocator)
{
    PFN_vkDestroyDebugUtilsMessengerEXT func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != NULL)
    {
        func(instance, debugMessenger, NULL);
    }
}

VkDebugUtilsMessengerCreateInfoEXT
createDebugMessengerCreateInfo()
{
    VkDebugUtilsMessengerCreateInfoEXT createInfo = {
        .sType              = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
        .messageSeverity    = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
                            | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
                            | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
        .messageType        = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
                            | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
                            | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
        .pfnUserCallback    = vulkanDebugCallback
    };
    return createInfo;
}

void
createVulkanDebugMessenger(VulkanContext *context)
{
    VkDebugUtilsMessengerCreateInfoEXT createInfo = createDebugMessengerCreateInfo();

    VkResult result = createDebugUtilsMessengerEXT(context->instance, &createInfo, NULL, &context->debugMessenger);

    if (result != VK_SUCCESS)
    {
        printf("Failed to create Vulkan debug messenger\n");
        exit(1);
    }
    printf("Vulkan debug messenger created.\n");
}

bool
checkValidationLayerSupport()
{
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, NULL);

    VkLayerProperties* availableLayers = malloc(sizeof(VkLayerProperties) * layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers);

    bool layerFound = false;
    for (uint32_t j = 0; j < layerCount; j++)
    {
        if (strcmp("VK_LAYER_KHRONOS_validation", availableLayers[j].layerName) == 0)
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

    free(availableLayers);
    return true;
}

void
createVulkanInstance(VulkanContext *context)
{
    if (context->enableValidationLayers && !checkValidationLayerSupport()) {
        fprintf(stderr, "Validation layers requested, but not available.\n");
        exit(1);
    }

    // Define application information
    VkApplicationInfo appInfo = {};
    appInfo.sType               = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName    = "Sky Lion";
    appInfo.applicationVersion  = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName         = "Sky Lion Engine";
    appInfo.engineVersion       = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion          = VK_API_VERSION_1_3;

    // Define instance create info
    VkInstanceCreateInfo createInfo = {};
    createInfo.sType            = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = createDebugMessengerCreateInfo();
    if (context->enableValidationLayers)
    {
        createInfo.enabledLayerCount    = context->validationLayerCount;
        createInfo.ppEnabledLayerNames  = &context->validationLayers;
        createInfo.pNext                = &debugCreateInfo;
    }
    else
    {
        createInfo.enabledLayerCount    = 0;
        createInfo.pNext                = NULL;
    }

    // Specify any required extensions, e.g., for window system integration
    Uint32 extensionCount = 0;
    const char** extensions = getVulkanExtensions(context, &extensionCount);
    createInfo.enabledExtensionCount   = extensionCount;
    createInfo.ppEnabledExtensionNames = extensions;

    // Create the Vulkan instance
    VkResult result = vkCreateInstance(&createInfo, NULL, &context->instance);

    if (extensions != NULL) { free(extensions); }

    if (result != VK_SUCCESS)
    {
        printf("Failed to create Vulkan instance\n");
        exit(1);
    }

    printf("Vulkan instance created.\n");
}

void
createRenderer()
{
#ifndef NDEBUG
    g_vulkanContext.enableValidationLayers = true;
    g_vulkanContext.validationLayerCount = 1;
    g_vulkanContext.validationLayers = "VK_LAYER_KHRONOS_validation";
#endif

    createVulkanInstance(&g_vulkanContext);
    if (g_vulkanContext.enableValidationLayers){ createVulkanDebugMessenger(&g_vulkanContext); }
    createSurface(&g_vulkanContext);
    createPhysicalDevice(&g_vulkanContext);
    createVulkanDevice(&g_vulkanContext);

}

void
destroyRenderer()
{
    vkDestroyDevice(g_vulkanContext.device, NULL);
    vkDestroySurfaceKHR(g_vulkanContext.instance, g_vulkanContext.surface, NULL);
    if (g_vulkanContext.enableValidationLayers) { destroyDebugUtilsMessengerEXT(g_vulkanContext.instance, g_vulkanContext.debugMessenger, NULL); }
    if (g_vulkanContext.extensions)
    {
        free(g_vulkanContext.extensions);
        g_vulkanContext.extensions = NULL;
    }
    vkDestroyInstance(g_vulkanContext.instance, NULL);
    printf("Vulkan instance destroyed.\n");
}