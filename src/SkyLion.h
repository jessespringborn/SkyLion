/*****************************************************************************************
 *
 *  Sky Lion Engine -
 *
 *****************************************************************************************/

#ifndef SKYLION_SRC_SKYLION_H
#define SKYLION_SRC_SKYLION_H

#ifndef NDEBUG
#define SKY_DEBUG
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

#include <cglm/cglm.h>
#include <cgltf.h>
#include <vulkan/vulkan.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>

#include "SkyWindow.h"
#include "SkyRenderer.h"

static const char *SKY_APP_NAME = "Sky Lion Game";
static const int SKY_APP_VERSION_MAJOR = 0;
static const int SKY_APP_VERSION_MINOR = 1;
static const int SKY_APP_VERSION_PATCH = 0;
static const char *SKY_ENGINE_NAME = "Sky Lion Engine";
static const int SKY_ENGINE_VERSION_MAJOR = 0;
static const int SKY_ENGINE_VERSION_MINOR = 1;
static const int SKY_ENGINE_VERSION_PATCH = 0;

// enum error codes
enum SKY_ERRORS {
    SKY_SUCCESS = 0,
    SKY_ERROR = 1,
    SKY_ERROR_NULL_POINTER = 2,
    SKY_ERROR_INVALID_ARGUMENT = 3,
    SKY_ERROR_INVALID_OPERATION = 4,
    SKY_ERROR_OUT_OF_MEMORY = 5,
    SKY_ERROR_UNKNOWN = 6,
    SKY_ERROR_SDL_INIT = 7,
    SKY_ERROR_WINDOW_CREATION = 8
};

#endif //SKYLION_SRC_SKYLION_H
