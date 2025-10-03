#pragma once 

#ifndef ENGINE_HPP
#define ENGINE_HPP

#include "core/macros.hpp"

#include "core/errors/assert.hpp"
#include "core/errors/errors.hpp"

namespace engine {
	
	core::error init();
	void run();
	void shutdown();

}

#endif