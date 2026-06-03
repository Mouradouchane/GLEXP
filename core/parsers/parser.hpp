#pragma once 

#ifndef CORE_PARSERS_HPP
#define CORE_PARSERS_HPP

#include "core/files/files.hpp"
#include "core/macros.hpp"

namespace core {

	/*
		note: assets manangers should use this, his job to take care of anything I/O related .
	*/

	namespace parse {

		// todo: return proper DS for every format
		void json(void* data , u32 size) NOEXP;
		void  xml(void* data, u32 size) NOEXP;
		void  ini(void* data, u32 size) NOEXP;

		// for our custom binary files
		void bin(void* data, u32 size, core::file_type bin_type) NOEXP;

	} // namespace parse end

} // namespace core end


#endif