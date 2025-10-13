#pragma once 

#ifndef ENGINE_HPP
#define ENGINE_HPP

#include "core/macros.hpp"
#include "core/assert.hpp"
#include "core/status/status.hpp"

namespace engine {
	
	core::error init();
	void run();
	void shutdown();

}

#endif