#pragma once

#ifndef ERRORS_CPP
#define ERRORS_CPP

#include <string>
#include <map>

#include "core/macros.hpp"
#include "core/types.hpp"
#include "status.hpp"

static const std::map<core::error, std::string> error_codes = {
	{core::error::none , "000"},

	// "init" ========================
	{core::error::init_function_failed , "ERR100"},
	
	// "shader" ========================
	{core::error::failed_to_save_shader, "ERR101"},
	{core::error::failed_to_load_shader, "ERR102"},
	{core::error::failed_to_compile_shader,"ERR103"},
	{core::error::failed_to_run_shader, "ERR104"},
	{core::error::shader_crash, "ERR105"},
	{core::error::failed_to_create_program, "ERR106"},
	{core::error::failed_to_create_vertex_shader,"ERR107"},
	{core::error::failed_to_create_fragment_shader,"ERR108"},

	// "files" =========================
	{core::error::file_not_found,			 "ERR109"} ,
	{core::error::failed_to_open_file,		 "ERR110"} ,
	{core::error::failed_to_create_file,	 "ERR111"} ,
	{core::error::failed_to_delete_file,	 "ERR112"} ,
	{core::error::failed_to_write_to_file,	 "ERR113"} ,
	{core::error::failed_to_read_from_file, "ERR114"} ,

	// "folders" ========================
	{core::error::folder_not_found,	     "ERR115"} ,
	{core::error::failed_to_open_folder,    "ERR116"} ,
	{core::error::failed_to_create_folder,  "ERR117"} ,
	{core::error::failed_to_delete_folder,  "ERR118"} ,

	// "configs" ========================
	{core::error::failed_to_load_ini_file, "ERR119"} ,
	{core::error::failed_to_save_ini_file, "ERR120"} ,

	// "models" ==========================
	{core::error::failed_to_process_model, "ERR121"} ,

	// "common" errors ====================
	{core::error::failed_to_load_resource , "ERR122"} ,
	{core::error::nullptr_object ,          "ERR123"} ,
};

static const std::map<core::error, std::string> error_strings = {
	{core::error::none , "no error"} ,

	// "init" ========================
	{core::error::init_function_failed , "init function failed"},

	// "shader" ========================
	{core::error::failed_to_save_shader, "failed to save shader"},
	{core::error::failed_to_load_shader, "failed to load shader"},
	{core::error::failed_to_compile_shader,"failed to compile shader"},
	{core::error::failed_to_run_shader, "failed to run shader"},
	{core::error::shader_crash, "shader crash"},
	{core::error::failed_to_create_program,"failed to create shader program"},
	{core::error::failed_to_create_vertex_shader,"failed to create vertex shader"},
	{core::error::failed_to_create_fragment_shader,"failed to create fragment shader"},

	// "files" ===================
	{core::error::file_not_found,			"file not found"},
	{core::error::failed_to_open_file,		"failed to open file"},
	{core::error::failed_to_create_file,	"failed to create file"},
	{core::error::failed_to_delete_file,	"failed to delete file"},
	{core::error::failed_to_write_to_file,	"failed to write to file"},
	{core::error::failed_to_read_from_file,"failed to read from file"},

	// "folders" ========================
	{core::error::folder_not_found,	   "folder not found"},
	{core::error::failed_to_open_folder,  "failed to open folder"},
	{core::error::failed_to_create_folder,"failed to create folder"},
	{core::error::failed_to_delete_folder,"failed to delete folder"},

	// "configs" ========================
	{core::error::failed_to_load_ini_file,"failed to load ini file"},
	{core::error::failed_to_save_ini_file,"failed to save ini file"},

	// models ========================
	{core::error::failed_to_process_model,"failed to process model"},

	// common errors
	{core::error::failed_to_load_resource , "failed to load resource"},
	{core::error::nullptr_object , "object is null-pointer"},

}; // error codes map end

namespace core {

	DLL_API error_info error_to_string(core::error error_code) {

		auto str_itr  = error_strings.find(error_code);

		if (str_itr == error_strings.end()) {
			return error_info{ "undefined","undefined", false };
		}

		return error_info{ "undefined", str_itr->second , false };
	}

} // namespace core

#endif