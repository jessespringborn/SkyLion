//
// Created by jesse on 2/6/24.
//

#include "SkyLion.h"

#include <vulkan/vulkan.h>

#include <vk_mem_alloc.h>

#include <SDL3/SDL_vulkan.h>

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

const char* g_validationLayers[] = {
    "VK_LAYER_KHRONOS_validation"
};

int g_vmaExtensions[] = {
    VK_KHR_dedicated_allocation,
    VK_KHR_bind_memory2,
    VK_KHR_maintenance4,
    VK_EXT_memory_budget,
    VK_EXT_memory_priority,
    VK_AMD_device_coherent_memory
};

const char* g_deviceExtensions[] = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

typedef struct QueueFamilyIndices
{
    bool graphicsFamilyHasValue;
    Uint32 graphicsFamily;
    bool presentFamilyHasValue;
    Uint32 presentFamily;
} QueueFamilyIndices;

typedef struct SwapChainSupportDetails
{
    VkSurfaceCapabilitiesKHR capabilities;
    Uint32 formatCount;
    VkSurfaceFormatKHR* formats;
    Uint32 presentModeCount;
    VkPresentModeKHR* presentModes;
} SwapChainSupportDetails;

#ifndef NDEBUG
const bool g_enableValidationLayers = true;
#else
const bool g_enableValidationLayers = false;
#endif

VkInstance                  g_vulkanInstance;
VkDebugUtilsMessengerEXT    g_vulkanDebugMessenger;
VkSurfaceKHR                g_vulkanSurface;
VkPhysicalDevice            g_vulkanPhysicalDevice;
VkDevice                    g_vulkanDevice;
VkQueue                     g_graphicsQueue;
VkQueue                     g_presentQueue;

VmaAllocator                g_vulkanMemoryAllocator;

void
createMemoryAllocator()
{
    
}

SwapChainSupportDetails
querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR* surface)
{
    SwapChainSupportDetails details;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, *surface, &details.capabilities);

    vkGetPhysicalDeviceSurfaceFormatsKHR(device, *surface, &details.formatCount, NULL);
    if (details.formatCount != 0)
    {
        details.formats = malloc(sizeof(VkSurfaceFormatKHR) * details.formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, *surface, &details.formatCount, details.formats);
    }

    vkGetPhysicalDeviceSurfacePresentModesKHR(device, *surface, &details.presentModeCount, NULL);
    if (details.presentModeCount != 0)
    {
        details.presentModes = malloc(sizeof(VkPresentModeKHR) * details.presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, *surface, &details.presentModeCount, details.presentModes);
    }

    return details;
}

QueueFamilyIndices
findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR* surface)
{
    struct QueueFamilyIndices indices = {
        .graphicsFamilyHasValue = false,
        .graphicsFamily = 0,
        .presentFamilyHasValue = false,
        .presentFamily = 0
    };

    Uint32 queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, NULL);
    VkQueueFamilyProperties* queueFamilies = malloc(sizeof(VkQueueFamilyProperties) * queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies);

    for (Uint32 i = 0; i < queueFamilyCount; i++)
    {
        if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            indices.graphicsFamilyHasValue = true;
            indices.graphicsFamily = i;
        }

        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, *surface, &presentSupport);
        if (presentSupport)
        {
            indices.presentFamilyHasValue = true;
            indices.presentFamily = i;
        }

        if (indices.graphicsFamilyHasValue && indices.presentFamilyHasValue)
        {
            break;
        }
    }

    free(queueFamilies);
    return indices;
}

void
createLogicalDevice()
{
    QueueFamilyIndices indices = findQueueFamilies(g_vulkanPhysicalDevice, &g_vulkanSurface);

    // TODO: Allow for different queue families

    float queuePriority = 1.0f;
    VkDeviceQueueCreateInfo queueCreateInfo = {};
    queueCreateInfo.sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = indices.graphicsFamily;
    queueCreateInfo.queueCount       = 1;
    queueCreateInfo.pQueuePriorities = &queuePriority;

    VkPhysicalDeviceFeatures deviceFeatures = {};

    VkDeviceCreateInfo createInfo = {};
    createInfo.sType                    = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.queueCreateInfoCount     = 1;
    createInfo.pQueueCreateInfos        = &queueCreateInfo;
    createInfo.pEnabledFeatures         = &deviceFeatures;
    createInfo.enabledExtensionCount    = sizeof(g_deviceExtensions) / sizeof(g_deviceExtensions[0]);
    createInfo.ppEnabledExtensionNames  = g_deviceExtensions;

    if (g_enableValidationLayers)
    {
        createInfo.enabledLayerCount = sizeof(g_validationLayers) / sizeof(g_validationLayers[0]);
        createInfo.ppEnabledLayerNames = g_validationLayers;
    }
    else
    {
        createInfo.enabledLayerCount = 0;
    }

    if (vkCreateDevice(g_vulkanPhysicalDevice, &createInfo, NULL, &g_vulkanDevice) != VK_SUCCESS)
    {
        printf("Failed to create logical device!\n");
        exit(1);
    }
    else
    {
        printf("Logical device created successfully!\n");
    }

    vkGetDeviceQueue(g_vulkanDevice, indices.graphicsFamily, 0, &g_graphicsQueue);
    vkGetDeviceQueue(g_vulkanDevice, indices.presentFamily, 0, &g_presentQueue);
}

bool
checkDeviceExtensionSupport(VkPhysicalDevice device)
{
    Uint32 extensionCount;
    vkEnumerateDeviceExtensionProperties(device, NULL, &extensionCount, NULL);
    VkExtensionProperties* availableExtensions = malloc(sizeof(VkExtensionProperties) * extensionCount);
    vkEnumerateDeviceExtensionProperties(device, NULL, &extensionCount, availableExtensions);

    for (Uint32 i = 0; i < sizeof(g_deviceExtensions) / sizeof(g_deviceExtensions[0]); i++)
    {
        bool extensionFound = false;
        for (Uint32 j = 0; j < extensionCount; j++)
        {
            if (strcmp(g_deviceExtensions[i], availableExtensions[j].extensionName) == 0)
            {
                extensionFound = true;
                break;
            }
        }
        if (!extensionFound)
        {
            free(availableExtensions);
            return false;
        }
    }

    free(availableExtensions);
    return true;
}

bool
isDeviceSuitable(VkPhysicalDevice device)
{
    QueueFamilyIndices indices = findQueueFamilies(device, &g_vulkanSurface);
    return indices.graphicsFamilyHasValue && indices.presentFamilyHasValue && checkDeviceExtensionSupport(device);
}

void
selectPhysicalDevice()
{
    Uint32 deviceCount = 0;
    vkEnumeratePhysicalDevices(g_vulkanInstance, &deviceCount, NULL);
    if (deviceCount == 0)
    {
        printf("Failed to find GPUs with Vulkan support!\n");
        exit(1);
    }
    VkPhysicalDevice* devices = malloc(sizeof(VkPhysicalDevice) * deviceCount);
    vkEnumeratePhysicalDevices(g_vulkanInstance, &deviceCount, devices);

    for (Uint32 i = 0; i < deviceCount; i++)
    {
        if (isDeviceSuitable(devices[i]))
        {
            g_vulkanPhysicalDevice = devices[i];
            break;
        }
    }

    if (g_vulkanPhysicalDevice == VK_NULL_HANDLE)
    {
        printf("Failed to find a suitable GPU!\n");
        exit(1);
    }

    free(devices);
}

void
createSurface()
{
    if (!SDL_Vulkan_CreateSurface(SLWindow, g_vulkanInstance, NULL, &g_vulkanSurface))
    {
        printf("Failed to create window surface!\n");
        exit(1);
    }
    else
    {
        printf("Window surface created successfully!\n");
    }
}

VkBool32
debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
{
    printf("Validation layer: %s\n", pCallbackData->pMessage);
    return VK_FALSE;
}

void
populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT* createInfo)
{
    createInfo->sType           = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo->messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
                                | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
                                | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo->messageType     = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
                                | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
                                | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo->pfnUserCallback = debugCallback;
    createInfo->pUserData       = NULL;
}

VkResult
CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger)
{
    PFN_vkCreateDebugUtilsMessengerEXT func = (PFN_vkCreateDebugUtilsMessengerEXT)
        vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
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
DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator)
{
    PFN_vkDestroyDebugUtilsMessengerEXT func = (PFN_vkDestroyDebugUtilsMessengerEXT)
        vkGetInstanceProcAddr(instance,"vkDestroyDebugUtilsMessengerEXT");
    if (func != NULL)
    {
        func(instance, debugMessenger, pAllocator);
    }
}

void
createDebugMessenger()
{

    VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
    populateDebugMessengerCreateInfo(&createInfo);


    if (CreateDebugUtilsMessengerEXT(g_vulkanInstance,
                                     &createInfo,
                                     NULL,
                                     &g_vulkanDebugMessenger)
                                     != VK_SUCCESS)
    {
        printf("Failed to set up debug messenger!\n");
        exit(1);
    }

    printf("Debug messenger created successfully!\n");

}

const char**
getRequiredExtensions(Uint32 *extensionCount)
{
    char const* const* extensionNames = SDL_Vulkan_GetInstanceExtensions(
        extensionCount);
    if (g_enableValidationLayers)
    {
        Uint32 count = *extensionCount;
        *extensionCount += 1;
        const char** extensions = malloc(sizeof(char*) * *extensionCount);
        for (Uint32 i = 0; i < count; i++)
        {
            extensions[i] = extensionNames[i];
        }
        extensions[count] = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;
        return extensions;
    }
    else
    {
        return extensionNames;
    }
}

bool
checkValidationLayerSupport()
{
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, NULL);

    VkLayerProperties* availableLayers = malloc(sizeof(VkLayerProperties) * layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers);

    for (uint32_t i = 0; i < sizeof(g_validationLayers) / sizeof(g_validationLayers[0]); i++)
    {
        bool layerFound = false;
        for (uint32_t j = 0; j < layerCount; j++)
        {
            if (strcmp(g_validationLayers[i], availableLayers[j].layerName) == 0)
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

void
createInstance()
{
    if (g_enableValidationLayers && !checkValidationLayerSupport())
    {
        printf("Validation layers requested, but not available!\n");
        exit(1);
    }

    VkApplicationInfo appInfo = {};
    appInfo.sType               = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName    = "Sky Lion";
    appInfo.applicationVersion  = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName         = "Sky Lion Engine";
    appInfo.engineVersion       = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion          = VK_API_VERSION_1_3;

    Uint32 extensionCount = 0;
    const char** extensionNames = getRequiredExtensions(&extensionCount);

    VkInstanceCreateInfo createInfo = {};
    createInfo.sType                    = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo         = &appInfo;
    createInfo.enabledExtensionCount    = extensionCount;
    createInfo.ppEnabledExtensionNames  = extensionNames;

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = {};
    if (g_enableValidationLayers)
    {
        createInfo.enabledLayerCount = sizeof(g_validationLayers) / sizeof(g_validationLayers[0]);
        createInfo.ppEnabledLayerNames = g_validationLayers;

        populateDebugMessengerCreateInfo(&debugCreateInfo);
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugCreateInfo;
    }
    else
    {
        createInfo.enabledLayerCount = 0;
        createInfo.pNext = NULL;
    }

    VkResult result = vkCreateInstance(&createInfo, NULL,&g_vulkanInstance);
    if (result != VK_SUCCESS)
    {
        printf("Failed to create Vulkan instance!\n");
        exit(1);
    }
    else
    {
        printf("Vulkan instance created successfully!\n");
    }

    if (g_enableValidationLayers)
    {
        free(extensionNames);
    }

}

/************************************************************
 *
 *  Create the renderer
 *
 ************************************************************/
void
createRenderer()
{
    createInstance();
    if (g_enableValidationLayers)
    {
        printf("Creating Vulkan instance with validation layers...\n");
        createDebugMessenger();
    }
    createSurface();
    selectPhysicalDevice();
    createLogicalDevice();

    createMemoryAllocator();
}

/************************************************************
 *
 *  Clean up
 *
 ************************************************************/
void
destroyRenderer()
{
    vkDestroySurfaceKHR(g_vulkanInstance, g_vulkanSurface, NULL);
    vkDestroyDevice(g_vulkanDevice, NULL);
    if (g_enableValidationLayers)
    {
        DestroyDebugUtilsMessengerEXT(g_vulkanInstance,g_vulkanDebugMessenger,NULL);
    }
    vkDestroyInstance(g_vulkanInstance, NULL);
}