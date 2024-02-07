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

#ifndef NDEBUG
const bool g_enableValidationLayers = true;
#else
const bool g_enableValidationLayers = false;
#endif

VkInstance                  g_vulkanInstance;
VkDebugUtilsMessengerEXT    g_vulkanDebugMessenger;
VkPhysicalDevice            g_vulkanPhysicalDevice;

VmaAllocator                g_vulkanMemoryAllocator;

void
createMemoryAllocator()
{
    
}

VkBool32
debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
              VkDebugUtilsMessageTypeFlagsEXT messageType,
              const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
              void* pUserData)
{
    printf("Validation layer: %s\n", pCallbackData->pMessage);
    return VK_FALSE;
}

void
populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT* createInfo, PFN_vkDebugUtilsMessengerCallbackEXT debugCallbackFunc)
{
    createInfo->sType           = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo->messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
                                | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
                                | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo->messageType     = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
                                | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
                                | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo->pfnUserCallback = debugCallbackFunc;
    createInfo->pUserData       = NULL;
}


VkResult
CreateDebugUtilsMessengerEXT(VkInstance instance,
                             const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
                             const VkAllocationCallbacks* pAllocator,
                             VkDebugUtilsMessengerEXT* pDebugMessenger)
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
DestroyDebugUtilsMessengerEXT(VkInstance instance,
                                   VkDebugUtilsMessengerEXT debugMessenger,
                                   const VkAllocationCallbacks* pAllocator)
{
    PFN_vkDestroyDebugUtilsMessengerEXT func = (PFN_vkDestroyDebugUtilsMessengerEXT)
        vkGetInstanceProcAddr(instance,"vkDestroyDebugUtilsMessengerEXT");
    if (func != NULL)
    {
        func(instance, debugMessenger, pAllocator);
    }
}

void
createDebugMessenger(VkInstance* instance, VkDebugUtilsMessengerEXT* debugMessenger)
{

    VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
    populateDebugMessengerCreateInfo(&createInfo, debugCallback);


    if (CreateDebugUtilsMessengerEXT(*instance,
                                     &createInfo,
                                     NULL,
                                     debugMessenger)
                                     != VK_SUCCESS)
    {
        printf("Failed to set up debug messenger!\n");
        exit(1);
    }

    printf("Debug messenger created successfully!\n");

}

const char**
getRequiredExtensions(Uint32 *extensionCount, bool enableValidationLayers)
{
    char const* const* extensionNames = SDL_Vulkan_GetInstanceExtensions(
        extensionCount);
    if (enableValidationLayers)
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
createInstance(VkInstance* instance, bool enableValidationLayers, const char** validationLayers, PFN_vkDebugUtilsMessengerCallbackEXT debugCallbackFunc)
{
    if (enableValidationLayers && !checkValidationLayerSupport())
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
    const char** extensionNames = getRequiredExtensions(&extensionCount, enableValidationLayers);

    VkInstanceCreateInfo createInfo = {};
    createInfo.sType                    = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo         = &appInfo;
    createInfo.enabledExtensionCount    = extensionCount;
    createInfo.ppEnabledExtensionNames  = extensionNames;

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = {};
    if (enableValidationLayers)
    {
        createInfo.enabledLayerCount = sizeof(g_validationLayers) / sizeof(g_validationLayers[0]);
        createInfo.ppEnabledLayerNames = validationLayers;

        populateDebugMessengerCreateInfo(&debugCreateInfo, debugCallbackFunc);
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugCreateInfo;
    }
    else
    {
        createInfo.enabledLayerCount = 0;
        createInfo.pNext = NULL;
    }

    VkResult result = vkCreateInstance(&createInfo, NULL,instance);
    if (result != VK_SUCCESS)
    {
        printf("Failed to create Vulkan instance!\n");
        exit(1);
    }
    else
    {
        printf("Vulkan instance created successfully!\n");
    }

    if (enableValidationLayers)
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
    createInstance(&g_vulkanInstance,g_enableValidationLayers,g_validationLayers, debugCallback);
    if (g_enableValidationLayers)
    {
        printf("Creating Vulkan instance with validation layers...\n");
        createDebugMessenger(&g_vulkanInstance,&g_vulkanDebugMessenger);
    }

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
    if (g_enableValidationLayers)
    {
        DestroyDebugUtilsMessengerEXT(g_vulkanInstance,
                                      g_vulkanDebugMessenger,
                                      NULL);
    }
    vkDestroyInstance(g_vulkanInstance, NULL);
}