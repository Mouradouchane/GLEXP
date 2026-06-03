#pragma once 

#ifndef CORE_FILES_HPP
#define CORE_FILES_HPP

#include "core/types.hpp"
#include "core/macros.hpp"
#include "core/strings/string.hpp"

namespace core {

	/*
		types of files or assets engine should support
	*/
	enum class file_type : u16 {
		unkown = 0,

		config,
		model,
		texture,
		material,
		vertex_shader,
		fragment_shader,
		
	};

	enum class file_permission: u8 {
		read  = 0b00000001,
		write = 0b00000010,
		all   = 0b00000011,
	};


	/*
		each game binary file or binary assets , will contain a header
		who describe data in file size , version , etc...
	*/
	namespace file_headers {

		struct config {

		};

		struct model {

		};

		struct texture {

		};

		struct material {

		};

		struct vertex_shader {

		};

		struct fragment_shader {

		};

	};
	// namespace file_headers end

	struct file_memory {
		void* ptr  = nullptr;
		u32   size = 0;
	};

	class file {
	private:
		STRING path = "";
		bool opened = false;
		file_permission permission;

	public:
		// constructor / destructor
		 file(STRING const& path, core::file_permission permission_ , bool auto_open = true) NOEXP;
		~file() NOEXP;

		// file public functions
		bool is_opened() NOEXP;
		bool open()      NOEXP;
		bool close()     NOEXP;

		file_memory read() NOEXP;
		bool        append(void* data, u32 size) NOEXP;

		// warning: - write remove all content in file then writes
		//          - use append if you want to add data to file
		bool write(void* data, u32 size) NOEXP;

	}; 
	// class file end

} 
// namespace core end


#endif