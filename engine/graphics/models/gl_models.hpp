#pragma once 

#ifndef GL_MODELS_HPP
#define GL_MODELS_HPP

/*
	todo: rework
*/
#if 0

#include <string>
#include <vector>

#include "libs/glew/glew.h"
//#include "glm.hpp"

#include "core/math/math.hpp"
#include "core/status/status.hpp"

#include "engine/graphics/gl_buffers.hpp"
#include "engine/textures/texture.hpp"
#include "engine/shaders/shader.hpp"
#include "engine/models/models.hpp"

class gl_mesh {
private:
	vao VAO;
	vbo VBO;
	ebo EBO; 

public:
	uint32_t indices_size = 0;
	std::string name = "unkown";

	// constructor's
	gl_mesh()  = default;
	gl_mesh(mesh* mesh_);
	// destructor
	~gl_mesh();

	// functions
	void render(shader const& shader_program);

	// todo: maybe we dont need bind/unbind
	void bind();
	void unbind();
};
// class gl_mesh end

class gl_model {
public:
	std::string name = "unkown";
	std::vector<gl_mesh> meshes;

	// constructor
	gl_model() = default;
	gl_model(model* model_);

	// destructor
	~gl_model();

	// render all meshes with same "shader program"
	void render(shader const& shader_program);
};
// class gl_model end 

#endif

#endif