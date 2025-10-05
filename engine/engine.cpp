#pragma once 

#ifndef ENGINE_CPP
#define ENGINE_CPP

#include <stdlib.h>
#include <tchar.h>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <thread>

#define  GLFW_INCLUDE_NONE
#include "libs/glfw/glfw3.h"
#include "libs/glew/glew.h"

#include "core/logger/logger.hpp"
#include "core/errors/assert.hpp"
#include "core/memory/memory.hpp"
#include "core/memory/memory_heap.hpp"
#include "core/data_structres/hashmaps/hash_map.hpp"

#include "engine/graphics/glerror_debug.hpp"
#include "engine/config/config.hpp"
#include "engine/textures/texture.hpp"
#include "engine/shaders/shader.hpp"
#include "engine/models/models.hpp"
#include "engine/resource/resource_manager.hpp"

#include "engine.hpp"

// engine variables
namespace engine {
	bool         running = true;
	std::string  title   = "GLEXP";
	GLFWwindow*  window  = nullptr;
}

static core::error init_glfw() {

	if ( !glfwInit() ) return core::error::failed_to_init_glfw;

	// get primary monitor information
	GLFWmonitor* primary_monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* monitor_modes = glfwGetVideoMode(primary_monitor);

	if (monitor_modes != nullptr) {
		config::screen::native_width  = uint16_t(monitor_modes->width);
		config::screen::native_height = uint16_t(monitor_modes->height);
		config::screen::refresh_rate  = uint16_t(monitor_modes->refreshRate);
	}
	primary_monitor = nullptr;
	monitor_modes   = nullptr;

	// set window hints
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	return core::error::none;
}

static core::error init_glew() {
	if (GLEW_OK != glewInit()) return core::error::failed_init_function;
	else {
		glViewport(0, 0, config::screen::width , config::screen::height);
		glClearColor(0, 0, 0, 1.0f); 

		return core::error::none; 
	}
}

static core::error create_window() {
	
	// create window
	engine::window = glfwCreateWindow(
		config::screen::width, 
		config::screen::height, 
		engine::title.c_str(), NULL, NULL
	);

	if (engine::window == nullptr) return core::error::failed_to_init_window;
	else {
		//glewExperimental = GL_TRUE;
		glfwMakeContextCurrent(engine::window);
		return core::error::none;
	}
}

namespace engine {

core::error init() {

	core::logger::init("glexp_engine", core::logger::verbosity_level::trace);

#if 0
	CORE_INFO ("test info  -> {}", 1);
	CORE_WARN ("test warn  -> {}", 1);
	CORE_DEBUG("test debug -> {}", 1);
	CORE_TRACE("test trace -> file:{} , line:{}", __FILE__ , __LINE__ );
	CORE_ERROR("test error -> file:{} , line:{}", __FILE__ , __LINE__);
	CORE_FATAL("test fatal -> file:{} , line:{}", __FILE__ , __LINE__);
#endif

	CRASH_IF_ERROR(init_glfw() , "failed to init glfw");

	// if loading configs from ini failed "it's fine" , we have default configs 
	config::load_configs_from_file("glexp.ini");

	create_window();

	init_glew();


	std::string opengl_version((const char*)glGetString(GL_VERSION));
	
	/*
	heap heap1( KB_TO_BYTE(1) , 32 , ALLOCATION_SECTION::GENERAL);

	void* _ptrs[8] = {};
	for (u32 i = 0; i < 8; i += 1) {
		_ptrs[i] = heap1.allocate(128);
	}

	u32 sz = heap1.allocated(MEMORY_UNIT::byte);
	DEBUG_BREAK;
	heap1.deallocate((byte*)_ptrs[7]);
	heap1.deallocate((byte*)_ptrs[2]);
	heap1.deallocate((byte*)_ptrs[6]);
	sz = heap1.allocated(MEMORY_UNIT::byte);
	heap1.allocate(128*2);
	*/

	// load resources based on ini file
	// core::error err = resource::load_resources("./resources.ini");

	// load_image textures 
	// TODO : make texture loader from file_list
	// if(init_textures() != core::error::none) return core::error::FAILED_TO_INIT_TEXTURES;
	
	return core::error::none;
}

void run() {
	
	/*
	// TODO : make texture or meshes handle "texture uints"
	glActiveTexture(GL_TEXTURE0);
	// set texture unit
	GLint sampler = glGetUniformLocation(program->id,"sampler");
	glUniform1i(sampler, 0);
	*/

	/*
			MAIN LOOP
	*/

	// todo: add time api in core library
	std::chrono::milliseconds sleep_time(15);
	
	while (engine::running) {
		// process events
		glfwPollEvents();

		// rendering
		GL_CHECK(glClear(GL_COLOR_BUFFER_BIT));
		
		/*
		test_model.meshes[0].bind();
		GL_CHECK(glDrawElements(GL_TRIANGLES, test_model.meshes[0].indices_size , GL_UNSIGNED_INT, 0));
		//GL_CHECK(glDrawArrays(GL_TRIANGLES, 0, 3));
		test_model.meshes[0].unbind();
		*/

		glfwSwapBuffers(window);

		if(glfwWindowShouldClose(window)) engine::running = false;

		// fps controll
		// TODO: make it calculated instead of hard-coded 15ms
		std::this_thread::sleep_for(sleep_time);
	}

}

// todo: engine shutdown stages need some work !
void shutdown() {

	// todo: detected memory leaks 
	

	// release librares stuff
	glfwTerminate();
	glfwDestroyWindow(engine::window);

	// todo: log to file if needed
	
	// todo: save game/engine stuff to files

}

} // engine namespace end

#endif