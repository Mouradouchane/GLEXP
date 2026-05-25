#pragma once 

#ifndef CORE_WINDOW_CPP
#define CORE_WINDOW_CPP

#include "core/logger/logger.hpp"
#include "window.hpp"

#ifdef DEBUG
	static auto _window_logger_cpp_ = CORE_GET_LOGGER(GUI_LOGGER);
#else
	static auto _window_logger_cpp_ = nullptr;
#endif

#define _LOGGER_ _window_logger_cpp_

// constructor
core::window::window(STRING const& name, window_size size, window_position position) NOEXP {

	this->_glfw_window_object_ = glfwCreateWindow(
		size.width, size.height,
		name.c_str(), nullptr, nullptr
	);

#ifdef DEBUG
	if (this->_glfw_window_object_ == nullptr) {
		CORE_FATAL_F(FAILED_TO_CREATE_WINDOW, this->_name_);
		return;
	}
	
	this->_name_ = name + "- DEBUG BUILD";
#else
	this->_name_ = name;
#endif

	this->change_position(position);
	this->change_size(size);
}

core::window::window(
	STRING const& name, window_size size, window_position position, window_resize resize, window_mode mode
) NOEXP {

	this->_glfw_window_object_ = glfwCreateWindow(
		size.width, size.height,
		name.c_str(), nullptr, nullptr
	);

#ifdef DEBUG
	if (this->_glfw_window_object_ == nullptr) {
		CORE_FATAL_F(FAILED_TO_CREATE_WINDOW, this->_name_);
		return;
	}

	this->_name_ = name + "- DEBUG BUILD";
#else
	this->_name_ = name;
#endif

	this->_resize_ = resize;

	this->change_position(position);
	this->change_size(size);

	this->handle_window_mode(mode , this->_fps_);
}


// destructor
core::window::~window() NOEXP {

	glfwDestroyWindow(this->_glfw_window_object_);
}

// public functions
window_position  core::window::get_position() NOEXP{
	return this->_position_;
}

window_size core::window::get_size() NOEXP {
	return this->_size_;
}

window_mode core::window::get_mode() NOEXP {
	return this->_mode_;
}

window_resize core::window::get_resize() NOEXP {
	return this->_resize_;
}

STRING core::window::get_name() NOEXP {
	return this->_name_;
}

window_internal_object core::window::get_internal_object() NOEXP {
	return window_internal_object{ this->_glfw_window_object_ };
}

window_internal_object core::window::get_internal_object() const NOEXP {
	return window_internal_object{ this->_glfw_window_object_ };
}

void core::window::change_name(STRING const& new_name) NOEXP {

#ifdef DEBUG
	if (this->_glfw_window_object_ == nullptr) {
		CORE_WARN_D(INVALID_WINDOW_OBJECT, this->_name_);
		return;
	}

	this->_name_ = new_name + "- DEBUG BUILD";
#else
	this->_name_ = new_name;
#endif

	glfwSetWindowTitle(this->_glfw_window_object_, this->_name_.c_str());
}

void core::window::change_size(window_size new_size) NOEXP {

#ifdef DEBUG
	if (this->_glfw_window_object_ == nullptr) {
		CORE_WARN_D(INVALID_WINDOW_OBJECT , this->_name_);
		return;
	}
#endif

	this->_size_ = new_size;

	glfwSetWindowSize(this->_glfw_window_object_, this->_size_.width, this->_size_.height);
}

void core::window::change_position(window_position new_poisiton) NOEXP {

#ifdef DEBUG
	if (this->_glfw_window_object_ == nullptr) {
		CORE_WARN_D(INVALID_WINDOW_OBJECT, this->_name_);
		return;
	}
#endif

	this->_position_ = new_poisiton;

	glfwSetWindowPos(this->_glfw_window_object_, this->_position_.x, this->_position_.y);
}

void core::window::change_frame_rate(u32 new_frame_rate) NOEXP {

	this->handle_window_mode(this->_mode_ , new_frame_rate);
}

void core::window::change_mode(window_mode new_mode) NOEXP {

	this->handle_window_mode(new_mode, this->_fps_);
}

// private helper functions
INLINE void core::window::handle_window_mode(window_mode new_mode, u32 new_fps) NOEXP {
	
	/*
		todo: debug this 
	*/

	this->_mode_ = new_mode;
	this->_fps_  = new_fps;

	// get the primary-monitor
	GLFWmonitor*    monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(monitor);
	
	// switch to screen mode
	switch (this->_mode_) {

		case window_mode::full_screen: {

			glfwSetWindowMonitor(
				this->_glfw_window_object_, monitor,
				0, 0,
				this->_size_.width, this->_size_.height, this->_fps_
			);

		} break;

		case window_mode::windowed: {

			const GLFWvidmode* mode = glfwGetVideoMode(monitor);

			// note: set hints before window creation, OR use glfwSetWindowMonitor matching current desktop settings
			glfwWindowHint(GLFW_RED_BITS,   mode->redBits);
			glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
			glfwWindowHint(GLFW_BLUE_BITS,  mode->blueBits);

			// note: if there is any bug in windowed mode change from _fps_ to mode->refreshRate
			glfwWindowHint(GLFW_REFRESH_RATE, this->_fps_);

			glfwSetWindowMonitor(
				this->_glfw_window_object_, monitor,
				0, 0,
				this->_size_.width, this->_size_.height,
				this->_fps_
			);

		} break;

		case window_mode::window: {

			glfwSetWindowMonitor(
				this->_glfw_window_object_, nullptr,
				this->_position_.x, this->_position_.y,
				this->_size_.width, this->_size_.height,
				this->_fps_
			);

		} break;

	}

}

#endif