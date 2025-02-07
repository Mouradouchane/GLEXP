#include "main.hpp"

int APIENTRY wWinMain(
    _In_     HINSTANCE h_instance,
    _In_opt_ HINSTANCE h_prev_instance,
    _In_     LPWSTR    lp_cmd_line,
    _In_     int       n_cmd_show
){

    if (!glfwInit()) return EXIT_FAILURE;

    // get primary monitor information
    GLFWmonitor* primary_monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* monitor_modes = glfwGetVideoMode(primary_monitor);
    
    if (monitor_modes != nullptr) {
        config::screen::native_width  = uint16_t(monitor_modes->width);
        config::screen::native_height = uint16_t(monitor_modes->height);
        config::screen::refresh_rate  = uint16_t(monitor_modes->refreshRate);
    }

    // set window hints
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    // load configs from ini file
    ERR result = config::load_configs_from_file("glexp.ini");

    // create window
    GLFWwindow* window = glfwCreateWindow(
        config::screen::width, 
        config::screen::height, 
        "GLEXP", NULL, NULL
    );

    if (window == nullptr) return EXIT_FAILURE;
    
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_press_handler);
    glfwSetMouseButtonCallback(window, mouse_click_handling);

    GLenum glew_init_result = glewInit();
    if (GLEW_OK != glew_init_result) return EXIT_FAILURE;

    std::srand(static_cast<unsigned>(std::time(0)));

    glViewport(0, 0, 800, 600);
    glClearColor(0, 0, 0,1.0f);

    // init data for shader
    init_data_for_shader();
    
    // init shader program
    shader program("shaders/shader.vert","shaders/shader.frag");
    if (program.last_error != G_ERROR::NO_ERR) return EXIT_FAILURE;

    if(init_textures() != G_ERROR::NO_ERR) return EXIT_FAILURE;

    program.use();

    glActiveTexture(GL_TEXTURE0);
    // set texture unit
    GLint sampler = glGetUniformLocation(program.id,"sampler");
    glUniform1i(sampler, 0);
    

    while (g_running) {

        // process events
        glfwPollEvents();
        
        // rendering
        glClear(GL_COLOR_BUFFER_BIT);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_BYTE, 0);
        glBindVertexArray(0);

        glfwSwapBuffers(window);

        // fps controll
        // todo: make it calculated instead of hard-coded 15ms
        Sleep(15);
    }

    glfwTerminate();
    return EXIT_SUCCESS;
}

/*
{
    glGenTextures(1, &wall.id);
    glBindTexture(GL_TEXTURE_2D, wall.id);
    // set the texture wrapping/filtering options (on currently bound texture)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(
        GL_TEXTURE_2D, 0, 
        GL_RGB, width, height, 0, 
        GL_RGB, GL_UNSIGNED_BYTE, data
    );
    glGenerateMipmap(GL_TEXTURE_2D);
}
*/