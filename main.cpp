#include <glad/glad.h> // include glad before glfw, glad already includes opengl headers
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <iostream>
#include <map>
#include <string>

#include "gen_shaders.h"
#include "util.hpp"

namespace my_window {
    constexpr size_t        height = 480;
    constexpr size_t        width  = 640;
    constexpr char const*   title  = "Hello World";
};

// callback defines
void event_error_callback(int code, const char* description);
void event_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void event_framebuffer_size_callback(GLFWwindow* window, int width, int height);

int main(void)
{
    GLFWwindow* window;

    // shader build output buffer
    int success;
    char infoLog[512];

    std::cout << "Compiled against GLFW " 
        << GLFW_VERSION_MAJOR << "." << GLFW_VERSION_MINOR << "." << GLFW_VERSION_REVISION << std::endl;

    // Initialize the library
    if (!glfwInit())
        return -1;

    //*==================================
    //* setup GLFW window hints before window creation, as otherwise they are not used
    //*==================================

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    //*==================================
    //* Setup opengl/glfw environment
    //*==================================

    // Create a windowed mode window and its OpenGL context
    window = glfwCreateWindow(my_window::width, my_window::height, my_window::title, NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        std::cout << "[GLAD] [ERR]: Failed to initialize GLAD" << std::endl;
        return -1;
    }
    glfwSwapInterval(1);                //
    glViewport(0, 0, my_window::width, my_window::height); // setup initial view port here

    //*==================================
    //* Setup event callbacks
    //*==================================
    
    glfwSetKeyCallback(window, &event_key_callback);
    glfwSetFramebufferSizeCallback(window, &event_framebuffer_size_callback);
    glfwSetErrorCallback(&event_error_callback);

    //*==================================
    //* Setup shaders
    //*==================================
    
    // create vertex shader & compile
    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertex_shader, NULL);
    glCompileShader(vertexShader);
    
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success); // check compile output
    if(!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "[GL] [ERR]: \"Failed to compile vertex shader\", " << infoLog << std::endl;
        glfwTerminate();
        return -1;
    }

    // create fragment shader & compile
    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragment_shader, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success); // check compile output
    if(!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "[GL] [ERR]: \"Failed to compile fragment shader\", " << infoLog << std::endl;
        glfwTerminate();
        return -1;
    }

    // create shader program
    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();

    // link fragment and vertex shader to shader program
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetShaderiv(shaderProgram, GL_COMPILE_STATUS, &success); // check link output
    if(!success) {
        glGetShaderInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "[GL] [ERR]: \"Failed to link shaders\", " << infoLog << std::endl;
        glfwTerminate();
        return -1;
    }

    // clean up vertex and fragment shader, as program has been compiled, so individual units are no longer necessary
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    //*==================================
    //* Create a triangle :D
    //*==================================

    float vertices[] = {
        0.5f, 0.5f, 0.0f, // top right
        0.5f, -0.5f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f, // bottom left
        -0.5f, 0.5f, 0.0f // top left
    };
    unsigned int indices[] = { // note that we start from 0!
        0, 1, 3, // first triangle
        1, 2, 3 // second triangle
    };

    // acquire a VAO ( vertex array object ) to store our triangle context into
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // acquire a VBO ( vertex buffer object ) to store our rectangle vertices in
    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO); // bind VBO to array buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // send data to graphics card

    // acquire an EBO ( element buffer object ) to store our rectangle vertex indices in
    unsigned int EBO;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW); // send data to graphics card

    // applies to our currently bound VBO
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //*==================================
    //* Actual render loop happens here
    //*==================================
    
    // Loop until the user closes the window
    while (!glfwWindowShouldClose(window)) {
        // Render here
        glClear(GL_COLOR_BUFFER_BIT);

        // imagine having multiple of these for now    
        glUseProgram(shaderProgram);        // use our shader for the triangle
        glBindVertexArray(VAO);             // use our rectangle VAO
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); // draw the actual rectangle ( interpret the VAO as a triangle )

        // Swap front and back buffers
        glfwSwapBuffers(window);

        // Poll for and process events
        glfwPollEvents();
    }

    //*==================================
    //* Clean up after render loop
    //*==================================

    glfwTerminate();
    return 0;
}


//==== event callbacks ====//

void event_error_callback(int code, const char* description)
{
    #ifdef DEBUG
    if (code != GLFW_NO_ERROR) {
        std::map<int, std::string> err_code = {
            {0x00000000, "GLFW_NO_ERROR           "},
            {0x00010001, "GLFW_NOT_INITIALIZED    "},
            {0x00010002, "GLFW_NO_CURRENT_CONTEXT "},
            {0x00010003, "GLFW_INVALID_ENUM       "},
            {0x00010004, "GLFW_INVALID_VALUE      "},
            {0x00010005, "GLFW_OUT_OF_MEMORY      "},
            {0x00010006, "GLFW_API_UNAVAILABLE    "},
            {0x00010007, "GLFW_VERSION_UNAVAILABLE"},
            {0x00010008, "GLFW_PLATFORM_ERROR     "},
            {0x00010009, "GLFW_FORMAT_UNAVAILABLE "},
            {0x0001000A, "GLFW_NO_WINDOW_CONTEXT  "},
        };

		std::cout << "[GLFW] [ERR][" << err_code[code] << "]";
		if (description)
        	std::cout << ": " << description << std::endl;
		else
			std::cout << std::endl;
    }
    #endif
}

void event_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    PARAM_UNUSED(scancode);
    PARAM_UNUSED(mods);
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void event_framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    PARAM_UNUSED(window);
    glViewport(0, 0, width, height);
}