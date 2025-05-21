#pragma once 

#ifndef MODELS_HPP
#define MODELS_HPP

#include <string>
#include <vector>
#include "errors.hpp"
#include "vectors.hpp"
#include "texture_2d.hpp"

class mesh {

public:
	std::string name = "unkown";

	std::vector<vertex> vertices;
	std::vector<uint32_t> indices;
	std::vector<texture_2d> textures;

	// constructor's
	mesh() = default;
	mesh(mesh const& mesh) = default;
	mesh(mesh&& mesh) = default;

	mesh(
		std::string const& mesh_name,
		std::vector<vertex>& _vertices,
		std::vector<uint32_t>& _indices,
		std::vector<texture_2d>& _textures
	);

	// destructor
	~mesh() = default;

};
// class mesh end


class model {

public:
	// ERR last_error = ERR::NO_ERR;
	std::string name = "unkown";
	std::vector<mesh*> meshes;

	// constructor's
	model() = default;
	model(model const& model) = default;
	model(model&& model) = default;

	// destructor
	~model();

};
// class model end

#endif