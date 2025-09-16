#pragma once 

#ifndef APP_HPP
#define APP_HPP

#include "core/errors/errors.hpp"

namespace application {
	
	ERR  init();
	ERR  run();
	void clean_up();

}

#endif