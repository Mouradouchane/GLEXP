#pragma once 

#ifndef MODELS_HPP
#define MODELS_HPP

#include <string>
#include <vector>
#include "glew/glew.h"
#include "errors.hpp"
#include "gl_buffers.hpp"

/*
	TODO : implement MODELS and MESHES
*/
class mesh {
private:
	vao VAO;
	vbo VBO;
	ebo EBO; 

public:
	std::string name = "unkown";
	 mesh() = default;
	~mesh();

	void bind();
	void unbind();
};

class model {
public:
	std::string name = "unkown";
	std::vector<mesh> mesh;

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