#pragma once 

#ifndef CORE_WINDOW_HPP
#define CORE_WINDOW_HPP

#include "core/macros.hpp"
#include "core/types.hpp"
#include "core/strings/string.hpp"

#include "libs/glfw/glfw3.h"

// window default macros
#define CORE_DEFAULT_WINDOW_X 0
#define CORE_DEFAULT_WINDOW_Y 0

#define CORE_DEFAULT_WINDOW_WIDTH  800
#define CORE_DEFAULT_WINDOW_HEIGHT 600

#define FPS
#define DEFAULT_FRAME_RATE 60

// window errors/warning macros
#define INVALID_WINDOW_OBJECT "window {} internal object is nullptr !"
#define FAILED_TO_CREATE_WINDOW "failed to create window '{}' interal object !"
/*
	few window types and enums
*/
struct window_position {
	u32 x;
	u32 y;
};

struct window_size {
	u32 width;
	u32 height;
};

struct window_internal_object {
	GLFWwindow* window = nullptr;
};

enum class window_mode : u8 {
	window      = 0,
	windowed    = 1,
	full_screen = 2
};

enum class window_resize : u8 {
	not_resizeable = 0,
	resizeable     = 1
};

namespace core {

	/*
		class window used to create gui window
		note : using GLFW for now
	*/

	DLL_API_CLASS window {
		private :
			// window variables
			GLFWwindow* _glfw_window_object_ = nullptr;

			STRING _name_ = "";

			window_position _position_ { 
				.x = CORE_DEFAULT_WINDOW_X,
				.y = CORE_DEFAULT_WINDOW_Y
			};

			window_size _size_ {
				.width  = CORE_DEFAULT_WINDOW_WIDTH,
				.height = CORE_DEFAULT_WINDOW_HEIGHT
			};

			window_mode   _mode_   = window_mode::window;
			window_resize _resize_ = window_resize::resizeable;

			u32 _fps_ = DEFAULT_FRAME_RATE;

		public:
			// constructor
			window(STRING const& name, window_size size, window_position position) NOEXP;
			window(
				STRING const& name, window_size size, window_position position, window_resize resize, window_mode mode
			) NOEXP;
			
			// destructor
			~window() NOEXP;

			// public functions
			window_position  get_position() NOEXP;
			window_size      get_size()     NOEXP;
			window_mode		 get_mode()     NOEXP;
			window_resize    get_resize()   NOEXP;
 			STRING           get_name()     NOEXP;

			window_internal_object get_internal_object() NOEXP;
			window_internal_object get_internal_object() const NOEXP;

			void change_name(STRING const& new_name) NOEXP;
			void change_size(window_size new_size)   NOEXP;
			void change_position(window_position new_poisiton) NOEXP;
			void change_frame_rate(u32 new_frame_rate) NOEXP;
			void change_mode(window_mode new_mode) NOEXP;


		private: // few private helper functions
			INLINE void handle_window_mode(window_mode new_mode , u32 new_fps) NOEXP;

	}; // class window end

} // namespace core end


#endif