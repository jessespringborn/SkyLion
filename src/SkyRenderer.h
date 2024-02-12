//
// Created by jesse on 2/11/24.
//

#ifndef SKYLION_SRC_SKYRENDERER_H
#define SKYLION_SRC_SKYRENDERER_H

#include <vulkan/vulkan.h>

typedef struct VulkanContext
{
    VkInstance instance;
    VkSurfaceKHR surface;
    VkPhysicalDevice physical_device;

#ifdef SKY_DEBUG
    VkDebugUtilsMessengerEXT            debug_messenger;
    PFN_vkCreateDebugUtilsMessengerEXT  create_debug_utils_messengerEXT;
    PFN_vkDestroyDebugUtilsMessengerEXT destroy_debug_utils_messengerEXT;
#endif
} VulkanContext;

/**
 * @brief Get the
 */
VulkanContext * get_context();

void create_renderer();

void destroy_renderer();

VkResult create_instance(VulkanContext *context);

VkResult create_surface(VulkanContext *context);

/**
 * @brief select preferred physical device
 * @param context a vulkan Context with the instance initialized
 */
VkResult select_physical_device(VulkanContext *context);


#define VK_CHECK(f, msg) \
{ \
    VkResult res = (f); \
    if (res != VK_SUCCESS) { \
        fprintf(stderr, "Fatal: %s - VkResult is %d in %s at line %d\n", msg, res, __FILE__, __LINE__); \
        assert(0); \
    } \
}

#endif //SKYLION_SRC_SKYRENDERER_H
