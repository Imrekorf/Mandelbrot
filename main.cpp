#include <glad/glad.h> // include glad before glfw, glad already includes opengl headers
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <iostream>
#include <map>
#include <string>
#include <stack>

#include <cmath>

#include "gen_shaders.h"
#include "util.hpp"

namespace my_window {
    constexpr size_t        height = 800;
    constexpr size_t        width  = 800;
    constexpr char const*   title  = "Mandlebrot";
    constexpr float         start_offset_x =  0.2;
    constexpr float         start_offset_y =  0.0;
    constexpr float         start_zoom     = -1.0;
};

#define TRANSLATE_ZOOM(level) ((zoom <= 1 ? abs(zoom) : (1.0 / zoom)))

// callback defines
void event_error_callback(int code, const char* description);
void event_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void event_mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void event_framebuffer_size_callback(GLFWwindow* window, int width, int height);
void event_scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

double offset_x = my_window::start_offset_x;
double offset_y = my_window::start_offset_y;
double zoom     = my_window::start_zoom;

// profiling
void countFPS();

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
    glfwSwapInterval(1);                                    //
    glViewport(0.5, 0.5, my_window::width, my_window::height);  // setup initial view port here

    //*==================================
    //* Setup event callbacks
    //*==================================
    
    glfwSetKeyCallback(window, &event_key_callback);
    glfwSetMouseButtonCallback(window, &event_mouse_button_callback);
    glfwSetScrollCallback(window, &event_scroll_callback);
    glfwSetFramebufferSizeCallback(window, &event_framebuffer_size_callback);
    glfwSetErrorCallback(&event_error_callback);

    //*==================================
    //* Setup shaders
    //*==================================
    
    // create vertex shader & compile
    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &GSV::vertex_shader, NULL);
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
    glShaderSource(fragmentShader, 1, &GSV::fragment_shader, NULL);
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
        -1.0f, -1.0f, -0.0f, // bottom left
         1.0f,  1.0f, -0.0f, // bottom right
        -1.0f,  1.0f, -0.0f, // top left
         1.0f, -1.0f, -0.0f  // top right
    };
    unsigned int indices[] = { // note that we start from 0!
        //  2---,1
        //  | .' |
        //  0'---3
        0, 1, 2, // first triangle
        0, 3, 1 // second triangle
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
    
    float timeValue = glfwGetTime();
    int u_time_loc = glGetUniformLocation(shaderProgram, GSV::u_time);
    int u_resolution_loc = glGetUniformLocation(shaderProgram, GSV::u_resolution);
    int u_offset_loc = glGetUniformLocation(shaderProgram, GSV::u_offset);
    int u_zoom_loc = glGetUniformLocation(shaderProgram, GSV::u_zoom);

    glUseProgram(shaderProgram);        // use our shader for the triangle
    glBindVertexArray(VAO);             // use our rectangle VAO
    glUniform1f(u_time_loc, timeValue);
    glUniform2f(u_resolution_loc, (float)my_window::width, (float)my_window::height);
    glUniform2f(u_offset_loc, (float)offset_x, (float)offset_y);
    glUniform1f(u_zoom_loc, (float)TRANSLATE_ZOOM(zoom));
    glEnable(GL_DEPTH_TEST);

    // Loop until the user closes the window
    while (!glfwWindowShouldClose(window)) {
        // Render here
        glClearColor(0.2f, 0.0f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // countFPS();

        // imagine having multiple of these for now    
        glUseProgram(shaderProgram);        // use our shader for the triangle
        glBindVertexArray(VAO);             // use our rectangle VAO
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); // draw the actual rectangle ( interpret the VAO as a triangle )

        timeValue = glfwGetTime();
        glUniform1f(u_time_loc, timeValue);
        glUniform2f(u_offset_loc, (float)offset_x, (float)offset_y);
        glUniform1f(u_zoom_loc, (float)TRANSLATE_ZOOM(zoom));

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

void event_mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    static std::stack<float> prev_diff_x, prev_diff_y;

    PARAM_UNUSED(mods);
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        // translate coordinates to center
        float diff_x, diff_y;
        diff_x = (xpos - my_window::width /2) / (my_window::width /2);
        diff_y = (ypos - my_window::height/2) / (my_window::height/2);

        std::cout << "diff (" << diff_x << ", " << diff_y 
                  << ") offset: (" << offset_x << ", " << offset_y << ")" 
                  << std::endl;

        // divide zoom constant by 2 as number range is -1.0 - 1.0
        float zoom_mult = TRANSLATE_ZOOM(zoom) / 2;
        diff_x *= zoom_mult;
        diff_y *= zoom_mult;

        offset_x -= diff_x;
        offset_y += diff_y;
        prev_diff_x.push(diff_x);
        prev_diff_y.push(diff_y);

        std::cout << "zoom_mult: " << zoom_mult 
                  << ", diff (" << diff_x << ", " << diff_y 
                  << ") offset: (" << offset_x << ", " << offset_y << ")" 
                  << std::endl;
    }
    
    if (button == GLFW_MOUSE_BUTTON_4 && action == GLFW_PRESS) {
        if (prev_diff_x.size() > 0) {
            offset_x = (offset_x + prev_diff_x.top());
            offset_y = (offset_y - prev_diff_y.top());
            prev_diff_x.pop();
            prev_diff_y.pop();
        }
    }
}

void event_scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    PARAM_UNUSED(window);
    PARAM_UNUSED(xoffset);
    zoom += yoffset;
    if (zoom == 0)
        zoom += yoffset;

    std::cout << "zoom: " << zoom << " zoom, translated: " << (zoom < 1 ? abs(zoom) : (1.0 / zoom)) << std::endl;
}

void event_framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    PARAM_UNUSED(window);
    glViewport(0.5, 0.5, width, height);
}

void countFPS()
{
    static int num_frames = 0;
    static float last_time = 0.0f;

    double current_time = glfwGetTime();
    num_frames++;
    if (current_time - last_time >= 1.0)
    {
        std::cout << num_frames << " fps" << std::endl;
        num_frames = 0;
        last_time += 1.0;
    }
}
 