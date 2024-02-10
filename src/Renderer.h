//
// Created by jesse on 2/6/24.
//

#ifndef SKYLION_SRC_RENDERER_H
#define SKYLION_SRC_RENDERER_H

#include <vulkan/vulkan.h>

#include <SDL3/SDL_vulkan.h>

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

typedef struct UniformBufferObject
{
    mat4 model;
    mat4 view;
    mat4 proj;
} UniformBufferObject;

/**
 * @brief Calls all required functions to create the renderer
 * @details Currently uses Vulkan API
 */
void createRenderer();

/**
 * @brief Calls all required functions to destroy the renderer
 * @details Currently uses Vulkan API
 */
void destroyRenderer();

void destroyVulkanDynamics();

void createVulkanStatics();

void createVulkanDynamics();

#include <stdio.h>
#include <stdlib.h>

char* readFile(const char* filename, size_t* outFileSize);

void populateVulkanDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT *createInfo);

VkResult createVulkanDebugUtilsMessengerEXT(VkInstance                                 instance,
                                        const VkDebugUtilsMessengerCreateInfoEXT   *pCreateInfo,
                                        const VkAllocationCallbacks                *pAllocator,
                                        VkDebugUtilsMessengerEXT                   *pDebugMessenger);

void destroyVulkanDebugUtilsMessengerEXT(VkInstance                     instance,
                                         VkDebugUtilsMessengerEXT       debugMessenger,
                                         const VkAllocationCallbacks    *pAllocator);

#endif //SKYLION_SRC_RENDERER_H
