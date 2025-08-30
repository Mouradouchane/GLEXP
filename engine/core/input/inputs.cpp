
#pragma once 

#ifndef INPUT_CPP
#define INPUT_CPP

#include "inputs.hpp"

void key_press_handler(GLFWwindow *window, int key, int scancode, int action,int mods) {

    switch (key) {
        case GLFW_KEY_ESCAPE: {
            g_running = false;
        } break;
    }

}

void mouse_click_handling(GLFWwindow *window, int button, int action,int mods) {

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {

        return;
    }

    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE) {

        return;
    }

}

#endif 
