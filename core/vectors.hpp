#pragma once 

#ifndef VECTORS_HPP
#define VECTORS_HPP

#include "core/assert.hpp"
#include "libs/glm/glm.hpp"

#define USE_GLM_VECTORS

#ifdef USE_GLM_VECTORS

typedef glm::vec2 vec2;
typedef glm::vec3 vec3;
typedef glm::vec4 vec4;

#else

typedef struct vec2 {
	float x, y;
};
typedef struct vec3 {
	float x, y, z;
};
typedef struct vec4 {
	float x, y, z, w;
};

#endif

struct vertex {
	vec3 position;
	vec3 normal;
	vec2 texture_coord;
};

#endif