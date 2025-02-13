#pragma once 

#ifndef MODELS_HPP
#define MODELS_HPP

#include <string>
#include <vector>
#include "glew/glew.h"
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
	vao handle;
	std::vector<mesh> mesh;

	// constructor
	model();
	// destructor
	~model();

};

namespace models_loader {

	void test_assimp(std::string const& file_path);

}

#endif