#pragma once

#ifndef ERRORS_HPP
#define ERRORS_HPP

#include <string>
#include "core/types.hpp"

namespace core {

	/*
		enum errors contain all the errors used in all the projects/tools
	*/
	enum class error : u16 {
		none = 0,

		// "init" ========================
		failed_init_function,
		failed_to_init_glfw,
		failed_to_init_glew,
		failed_to_init_window,
		// ===============================

		// "shader" ========================
		invalid_shader_program,
		failed_to_compile_vertex_shader,
		failed_to_compile_fragment_shader,
		// linking
		failed_to_link_program, 
		// create
		failed_to_create_program,
		failed_to_create_vertex_shader,
		failed_to_create_fragment_shader,
		// ==================================

		// "files" ===================
		file_not_found,
		failed_to_open_file,
		failed_to_create_file,
		failed_to_delete_file,
		failed_to_write_to_file,
		failed_to_read_from_file,
		// ==================================

		// "folders" ========================
		folder_not_found,
		failed_to_open_folder,
		failed_to_create_folder,
		failed_to_delete_folder,
		// ==================================

		// "images" ========================
		failed_to_load_image,
		nullptr_image,
		// ==================================

		// "textures" ========================
		texture_buffer_nullptr,
		failed_to_load_texture,
		failed_to_load_texture_buffer,
		failed_to_init_texture,
		failed_to_init_textures,
		failed_to_load_some_textures,
		// ==================================

		// "configs" ========================
		failed_to_load_ini_file,
		failed_to_write_to_ini_file,
		unsupported_resolution,
		unsupported_fov_value,
		// ==================================

		// models ========================
		failed_to_load_model,
		failed_to_process_model,
		nullptr_model_object,
		// ==================================

	}; // enum class errors end

} // namespace core end

#endif
