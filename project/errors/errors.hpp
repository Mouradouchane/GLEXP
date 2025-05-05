#pragma once

#ifndef ERRORS_HPP
#define ERRORS_HPP

#include <stdint.h>
#include <string>

/*
	NOTE: if youre not using visual studio , make sure to change __debugbreak 
	with the one working for your IDE/DEBUGGER
*/ 
#define DEBUG_BREAK __debugbreak;

/*
	all the errors is here in ERR enum
*/
enum class ERR : uint16_t {
	NO_ERR = 0,

	// assert errors
	ASSERT_STOP,

	// application errors
	FAILED_TO_INIT_APPLICATION,
	FAILED_TO_INIT_GLFW,
	FAILED_TO_INIT_GLEW,
	FAILED_TO_CREATE_WINDOW,

	// main loop errors
	INVALID_SHADER_PROGRAM,

	// "shader compile" errors
	FAILED_TO_COMPILE_VERTEX_SHADER,
	FAILED_TO_COMPILE_FRAGMENT_SHADER,

	// "shader linking" errors
	FAILED_TO_LINK_PROGRAM,

	// "shader create" errors
	FAILED_TO_CREATE_PROGRAM,
	FAILED_TO_CREATE_VERTEX_SHADER,
	FAILED_TO_CREATE_FRAGMENT_SHADER,

	// "files" errors
	FAILED_TO_LOAD_SHADER_FILE,

	// "textures" errors
	FAILED_TO_INIT_TEXTURES,
	FAILED_TO_LOAD_SOME_TEXTURE,

	// "configs" errors
	FAILED_TO_LOAD_INI_FILE,
	FAILED_TO_WRITE_TO_INI_FILE,
	UNSUPPORTED_RESOLUTION,
	UNSUPPORTED_FOV_VALUE,

	// models errors
	FAILED_TO_LOAD_MODEL,
	FAILED_TO_PROCESS_MODEL,
	NULLPTR_MODEL_OBJET,
};

/*
	NOTE: throw error will stop current thread execution,
	while throw warning not 
*/
#ifdef DEBUG
	void display_error_messagebox(
		std::string const& message,
		std::string const& function,
		std::string const& file,
		uint32_t line
	);

	void display_warn_messagebox(
		std::string const& message,
		std::string const& function,
		std::string const& file,
		uint32_t line
	);
#else 
	void display_error_messagebox( std::string const& message );
	void display_warn_messagebox( std::string const& message );
#endif



#ifdef DEBUG

	/*
		THROW ERROR MESSAGE + FORCE EXIT
	*/
	#define EXIT_AT_ERR(EXPRESSION , ERROR_MESSAGE) \
			if(EXPRESSION != ERR::NO_ERR){\
				display_error_messagebox(\
					ERROR_MESSAGE," ",__FILE__,__LINE__ \
				); exit(-1); \
			}

	#define EXIT_AT_FALSE(EXPRESSION , ERROR_MESSAGE) \
			if( ! EXPRESSION ){\
				display_error_messagebox(\
					ERROR_MESSAGE," ",__FILE__,__LINE__ \
				); exit(-1); \
			}

	/*
		THROW ERROR MESSAGE BOX MACROS
	*/
	#define DISPLAY_ERROR( FUNCTION , ERROR_MESSAGE ) \
		if( FUNCTION != ERR::NO_ERR ) {\
			display_error_messagebox(ERROR_MESSAGE,#FUNCTION,__FILE__,__LINE__);\
			DEBUG_BREAK \
		}

	#define DISPLAY_ERROR_EXP( EXPRESSION , ERROR_MESSAGE )\
		if( ! EXPRESSION ){\
			display_error_messagebox(ERROR_MESSAGE,#EXPRESSION,__FILE__,__LINE__);\
			DEBUG_BREAK \
		}

	/*
		THROW WARNING MESSAGE BOX MACROS
	*/
	#define DISPLAY_WARN( FUNCTION , WARNING_MESSAGE ) \
		if( FUNCTION != ERR::NO_ERR ) {\
			display_warn_messagebox(WARNING_MESSAGE,#FUNCTION,__FILE__,__LINE__);\
			DEBUG_BREAK \
		}

	#define DISPLAY_WARN_EXP( EXPRESSION , WARNING_MESSAGE ) \
		if( ! EXPRESSION ) {\
			display_warn_messagebox(WARNING_MESSAGE,#EXPRESSION,__FILE__,__LINE__);\
			DEBUG_BREAK \
		}

#else   /* =========== NONE DEBUG VERSION =========== */

	/*
		THROW ERROR MESSAGE + FORCE EXIT
	*/
	#define EXIT_AT_ERR(EXPRESSION , ERROR_MESSAGE) \
			if(EXPRESSION != ERR::NO_ERR){ \
				display_error_messagebox(ERROR_MESSAGE); \
				exit(-1); \
			}

	#define EXIT_AT_FALSE(EXPRESSION , ERROR_MESSAGE) \
			if( ! EXPRESSION ){ \
				display_error_messagebox(ERROR_MESSAGE); \
				exit(-1); \
			}

	/*
		THROW ERROR MESSAGE BOX MACROS
	*/
	#define DISPLAY_ERROR( FUNCTION , ERROR_MESSAGE ) \
		if( FUNCTION != ERR::NO_ERR ) {\
			display_error_messagebox(ERROR_MESSAGE);\
		}

	#define DISPLAY_ERROR_EXP( EXPRESSION , ERROR_MESSAGE )\
		if( ! EXPRESSION ){\
			display_error_messagebox(ERROR_MESSAGE);\
		}

	/*
		THROW WARNING MESSAGE BOX MACROS
	*/
	#define DISPLAY_WARN( FUNCTION , WARNING_MESSAGE ) \
		if( FUNCTION != ERR::NO_ERR ) {\
			display_warn_messagebox(WARNING_MESSAGE);\
		}

	#define DISPLAY_WARN_EXP( EXPRESSION , WARNING_MESSAGE ) \
		if( ! EXPRESSION ) {\
			display_warn_messagebox(WARNING_MESSAGE);\
		}
#endif

#endif
