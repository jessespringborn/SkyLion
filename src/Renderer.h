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

typedef struct QueueFamilyIndices
{
    uint32_t graphicsFamily; // Index of the graphics queue family
    uint32_t presentFamily;  // Index of the present queue family
    bool graphicsFamilyFound;
    bool presentFamilyFound;
} QueueFamilyIndices;

typedef struct VulkanContext
{
    // The Vulkan instance, the main handle to the Vulkan library and the first object you create.
    VkInstance instance;

    QueueFamilyIndices queueFamilyIndices;

    // True when running in debug mode
    bool enableValidationLayers;

    uint32_t validationLayerCount;

    const char* validationLayers;

    // The number of extensions required by the instance.
    Uint32 extensionCount;

    // The names of the extensions required by the instance.
    const char** extensions;

    // The debug messenger, used to report errors and warnings from the validation layers.
    VkDebugUtilsMessengerEXT debugMessenger;

    // The selected physical device (GPU) that supports the features we need.
    VkPhysicalDevice physicalDevice;

    // The logical device, used to interface with the physical device.
    VkDevice device;

    // The queue that supports graphics commands.
    VkQueue graphicsQueue;

    // The surface to which we will present our rendered images, typically tied to a windowing system.
    VkSurfaceKHR surface;

    // The queue that supports presentation to the window surface.
    VkQueue presentQueue;

    // The swap chain that manages the images used for presenting to the surface.
    VkSwapchainKHR swapChain;

    // The format of the swap chain images, typically matched to the surface's format.
    VkFormat swapChainImageFormat;

    // The size (width, height) of the swap chain images, usually the size of the window.
    VkExtent2D swapChainExtent;

    // Array of handles to the images in the swap chain.
    VkImage* swapChainImages;
    uint32_t swapChainImageCount; // The number of images in the swap chain.

    // Array of image views, one for each image in the swap chain. Image views represent how to access the image data.
    VkImageView* swapChainImageViews;

    // A render pass defines how the attachments (images) are used during rendering.
    VkRenderPass renderPass;

    // The pipeline layout object, which describes the uniform and push constants used by the pipeline.
    VkPipelineLayout pipelineLayout;

    // The graphics pipeline, which configures the fixed-function stages and programmable shaders for rendering.
    VkPipeline graphicsPipeline;

    // Array of framebuffers, one for each swap chain image. A framebuffer references all of the VkImageView objects that represent the attachments.
    VkFramebuffer* swapChainFramebuffers;

    // A command pool that manages the memory used to store the command buffers.
    VkCommandPool commandPool;

    // Array of command buffers, one for each framebuffer. Command buffers record all the operations to be performed during rendering.
    VkCommandBuffer* commandBuffers;

    // Semaphores and fence for synchronizing the rendering and presentation operations.
    VkSemaphore imageAvailableSemaphore; // Signaled when an image is available for rendering.
    VkSemaphore renderFinishedSemaphore; // Signaled when rendering is finished and the image is ready for presentation.
    VkFence inFlightFence; // Used to wait for the rendering of a frame to be completed before reusing its resources.

    // Shader modules contain the compiled shader code. One for the vertex shader and one for the fragment shader.
    VkShaderModule vertShaderModule;
    VkShaderModule fragShaderModule;

    // Optional: Additional fields might be required for more complex scenarios, such as dynamic state information, depth-stencil buffers, multisampling, etc.

} VulkanContext;


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

#endif //SKYLION_SRC_RENDERER_H
