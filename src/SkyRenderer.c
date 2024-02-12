//
// Created by jesse on 2/11/24.
//

#include "SkyLion.h"

#ifdef SKY_DEBUG

static VKAPI_ATTR VkBool32 VKAPI_CALL
vulkan_debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT      message_severity,
                      VkDebugUtilsMessageTypeFlagsEXT             message_type,
                      const VkDebugUtilsMessengerCallbackDataEXT  *callback_data,
                      void                                        *user_data)
{
    switch (message_severity)
    {
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
            //printf("Verbose: %s\n", callback_data->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
            //printf("Info: %s\n", callback_data->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            fprintf(stderr, "Warning: %s\n", callback_data->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            fprintf(stderr, "Error: %s\n", callback_data->pMessage);
            break;
        default:
            break;
    }
    return VK_FALSE;
}

VkResult
setup_vulkan_debug()
{
    get_context()->create_debug_utils_messengerEXT = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(get_context()->instance, "vkCreateDebugUtilsMessengerEXT");
    get_context()->destroy_debug_utils_messengerEXT = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(get_context()->instance, "vkDestroyDebugUtilsMessengerEXT");

    VkDebugUtilsMessengerCreateInfoEXT debug_utils_messenger_create_info = {};
    debug_utils_messenger_create_info.sType             = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debug_utils_messenger_create_info.messageSeverity   = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
                                                        | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT
                                                        | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
                                                        | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    debug_utils_messenger_create_info.messageType       = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
                                                        | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
                                                        | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    debug_utils_messenger_create_info.pfnUserCallback   = vulkan_debug_callback;
    debug_utils_messenger_create_info.pUserData         = NULL;

    return get_context()->create_debug_utils_messengerEXT(get_context()->instance, &debug_utils_messenger_create_info, NULL, &get_context()->debug_messenger);
}

#endif


VulkanContext *
get_context()
{
    static VulkanContext context;
    return &context;
}

void
create_renderer()
{
    VK_CHECK(create_instance(get_context()), "Failed to create Vulkan instance");
#ifdef SKY_DEBUG
    VK_CHECK(setup_vulkan_debug(), "Failed to set up Vulkan debug");
#endif
    VK_CHECK(select_physical_device(get_context()), "Failed to select physical device");

}

void
destroy_renderer()
{
    vkDestroySurfaceKHR(get_context()->instance, get_context()->surface, NULL);
#ifdef SKY_DEBUG
    get_context()->destroy_debug_utils_messengerEXT(get_context()->instance, get_context()->debug_messenger, NULL);
#endif
    vkDestroyInstance(get_context()->instance, NULL);
}

VkResult
create_instance(VulkanContext *context)
{
#ifdef SKY_DEBUG
    const char  *instance_extensions[2]      = { VK_KHR_SURFACE_EXTENSION_NAME, VK_EXT_DEBUG_UTILS_EXTENSION_NAME };
    const int   instance_extensions_count     = 2;
    const char  *validation_layer_names[1]    = { "VK_LAYER_KHRONOS_validation" };
    const int   validation_layer_count        = 1;
#else
    const char* instance_extensions[2] = { VK_KHR_SURFACE_EXTENSION_NAME };
        const int instance_extensions_count = 1;
        const char * validation_layer_names[0];
        const int validation_layer_count = 0;
#endif

    VkApplicationInfo app_info = {};
    app_info.sType               = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pApplicationName    = SKY_APP_NAME;
    app_info.applicationVersion  = VK_MAKE_VERSION(SKY_APP_VERSION_MAJOR, SKY_APP_VERSION_MINOR, SKY_APP_VERSION_PATCH);
    app_info.pEngineName         = SKY_ENGINE_NAME;
    app_info.engineVersion       = VK_MAKE_VERSION(SKY_ENGINE_VERSION_MAJOR, SKY_ENGINE_VERSION_MINOR, SKY_ENGINE_VERSION_PATCH);
    app_info.apiVersion          = VK_API_VERSION_1_2;

    VkInstanceCreateInfo instance_create_info = {};
    instance_create_info.sType                    = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instance_create_info.pApplicationInfo         = &app_info;
    instance_create_info.enabledExtensionCount    = instance_extensions_count;
    instance_create_info.ppEnabledExtensionNames  = instance_extensions;
    instance_create_info.enabledLayerCount        = validation_layer_count;
    instance_create_info.ppEnabledLayerNames      = validation_layer_names;

    return vkCreateInstance(&instance_create_info, NULL, &context->instance);
}

VkResult
createSurface(VulkanContext *context)
{
    if (!SDL_Vulkan_CreateSurface(get_window()->window, get_context()->instance, NULL, &get_context()->surface))
    {
        printf("Failed to create window surface!\n");
        return VK_ERROR_SURFACE_LOST_KHR;
    }
    else
    {
        printf("Window surface created successfully!\n");
        return VK_SUCCESS;
    }
}

VkResult
select_physical_device(VulkanContext *context)
{
    uint32_t gpu_count = 0;
    VK_CHECK(vkEnumeratePhysicalDevices(get_context()->instance, &gpu_count, NULL), "Failed to enumerate physical devices");
    assert(gpu_count > 0);

    VkPhysicalDevice physical_devices[gpu_count];
    VK_CHECK(vkEnumeratePhysicalDevices(get_context()->instance, &gpu_count, physical_devices), "Failed to enumerate physical devices")

    uint32_t selected_device = 0;
    for (uint32_t i = 0; i < gpu_count; i++)
    {
        VkPhysicalDeviceProperties device_properties;
        vkGetPhysicalDeviceProperties(physical_devices[i], &device_properties);
        if (device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
        {
            selected_device = i;
            break;
        }
    }

    get_context()->physical_device = physical_devices[selected_device];

    return VK_SUCCESS;
}
