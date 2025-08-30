#pragma once

#ifndef INPUT_HPP
#define INPUT_HPP

#include "globals.hpp"
#include "shader.hpp"
#include "glfw/glfw3.h"

void key_press_handler(GLFWwindow *window, int key, int scancode, int action, int mods);
void mouse_click_handling(GLFWwindow *window, int button, int action, int mods);

#endif
