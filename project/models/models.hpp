#pragma once 

#ifndef MODELS_HPP
#define MODELS_HPP

#include <string>
#include <vector>
#include "glew/glew.h"
#include "glm.hpp"
#include "errors.hpp"
#include "gl_buffers.hpp"
#include "texture_2d.hpp"

typedef glm::vec3 vec3;
typedef glm::vec2 vec2;

struct vertex {
	vec3 position;
	vec3 normal;
	vec2 texture_coord;
};

struct mesh_data{
	std::vector<vertex> vertices;
	std::vector<uint32_t> indices;
	std::vector<texture_2d> textures;
	// constructor's
	mesh_data() = default;
	mesh_data(
		std::vector<vertex>& _vertices,
		std::vector<uint32_t>& _indices,
		std::vector<texture_2d>& _textures
	);
	// destructor
	~mesh_data() = default;
};

class mesh {
private:
	vao VAO;
	vbo VBO;
	ebo EBO; 

public:
	uint32_t indices_size = 0;
	std::string name = "unkown";
	mesh() = default;
	mesh(mesh_data* data);
	~mesh();

	void bind();
	void unbind();
};

class model {
public:
	std::string name = "unkown";
	std::vector<mesh> meshs;

	// constructor
	model() = default;
	//model(std::string const& model_file_path , bool load_automatically = true);

	// destructor
	~model();

	// static methods for models
	static ERR load_model(
		std::string const& model_file_path, 
		model* dest_model_object
	);
};

#endif