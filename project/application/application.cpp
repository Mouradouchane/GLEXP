#pragma once 

#ifndef APP_CPP
#define APP_CPP

#include "application.hpp"

#include <stdlib.h>
//#include <malloc.h>
//#include <memory.h>
#include <tchar.h>
//#include <iostream>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <thread>

#include "glew/glew.h"
#include "glfw/glfw3.h"

#include "assert.hpp"
#include "config.hpp"
#include "globals.hpp"
#include "inputs.hpp"
#include "texture_2d.hpp"
#include "shader.hpp"

namespace application {

GLFWwindow*  window  = nullptr;
shader* program = nullptr;

static ERR init_glfw() {

	if ( !glfwInit() ) return ERR::FAILED_TO_INIT_GLFW;

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

	return ERR::NO_ERR;
}

static ERR init_glew() {
	if (GLEW_OK != glewInit()) return ERR::FAILED_TO_INIT_GLEW;
	else return ERR::NO_ERR;
}

static ERR create_window() {
	// set window hints
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	// create window
	window = glfwCreateWindow(
		config::screen::width, 
		config::screen::height, 
		"GLEXP", NULL, NULL
	);

	if (window == nullptr) return ERR::FAILED_TO_CREATE_WINDOW;
	else {
		glfwMakeContextCurrent(window);
		return ERR::NO_ERR;
	}
}

static ERR init_inputs_handling() {
	// setup keyboard+mouse handling
	glfwSetKeyCallback(window, key_press_handler);
	glfwSetMouseButtonCallback(window, mouse_click_handling);
}

ERR init() {

	ASSERT_ERR(init_glfw());

	// it's ok if its failed to load configs from ini file
	// we have default configs 
	config::load_configs_from_file("glexp.ini");
	
	ASSERT_ERR(create_window());

	ASSERT_ERR(init_inputs_handling());

	ASSERT_ERR(init_inputs_handling());

	ASSERT_ERR(init_glew());

	// setup view port
	glViewport(0, 0, config::screen::width , config::screen::height);
	// screen clear color "black"
	glClearColor(0, 0, 0,1.0f); 

	// load data for shader program
	init_data_for_shader();

	// setup shader program
	program = new shader("shaders/shader.vert","shaders/shader.frag");
	if (program->last_error != ERR::NO_ERR) return ERR::FAILED_TO_CREATE_PROGRAM;

	// load textures for shader program
	if(init_textures() != ERR::NO_ERR) return ERR::FAILED_TO_INIT_TEXTURES;

	return ERR::NO_ERR;
}

ERR run() {

	if (program == nullptr || program->last_error != ERR::NO_ERR) {
		return ERR::INVALID_SHADER_PROGRAM;
	}

	program->use();

	// TODO : make texture objects handle "texture uints"
	glActiveTexture(GL_TEXTURE0);

	// set texture unit
	GLint sampler = glGetUniformLocation(program->id,"sampler");
	glUniform1i(sampler, 0);

	/*
			MAIN LOOP
	*/
	std::chrono::milliseconds sleep_time(15);
	
	while (g_running) {
		// process events
		glfwPollEvents();

		// rendering
		glClear(GL_COLOR_BUFFER_BIT);

		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_BYTE, 0);
		glBindVertexArray(0);

		glfwSwapBuffers(window);

		g_running = (glfwWindowShouldClose(window)) ? false : true;

		// fps controll
		// TODO: make it calculated instead of hard-coded 15ms
		std::this_thread::sleep_for(sleep_time);
	}

	return ERR::NO_ERR;
}

void clean_up() {

	// free resource
	glfwTerminate();

	glfwDestroyWindow(application::window);
	window = nullptr;

	delete program;

	// save changes to files

}

} // application namespace end

#endif