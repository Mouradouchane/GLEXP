#pragma once 

#ifndef VECTORS_HPP
#define VECTORS_HPP

#include "glm.hpp"

typedef glm::vec2 vec2;
typedef glm::vec3 vec3;
typedef glm::vec4 vec4;

struct vertex {
	vec3 position;
	vec3 normal;
	vec2 texture_coord;
};

#endif