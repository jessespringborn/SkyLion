//
// Created by jesse on 2/9/24.
//

#ifndef SKYLION_SRC_MODEL_H
#define SKYLION_SRC_MODEL_H

#include "SkyLion.h"

typedef struct PushConstantData
{
    mat4 model;
    mat4 view;
    mat4 proj;
} PushConstantData;

typedef struct Vertex
{
    vec3		pos;			// Position
    vec3		normal;			// not currently used
    vec2		uv;				// Texture coordinate
    vec3		color;			// not currently used
    vec4		jointIndices;	// glTF supports 4 indices per joint
    vec4		jointWeights;	// glTF supports 4 weights per joint
} Vertex;


#endif //SKYLION_SRC_MODEL_H
