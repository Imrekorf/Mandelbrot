#include <glad/glad.h> // include glad before glfw, glad already includes opengl headers
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <iostream>
#include <iomanip>
#include <map>
#include <string>
#include <deque>

#include <cmath>
#include <cstring>
#include <vector>

#include <fstream>

#include "gen_shaders.h"
#include "util.hpp"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-result"
#include "ttmath.h"
#pragma GCC diagnostic pop
#include "BigNum/BigUInt.h"
#include "BigNum/BigInt.h"
#include "BigNum/BigFloat.h"
#include "BigNum/BigNumWrapper.h"

class glsl_big_uint_t {
        struct {
            int size;
            int table;
        } uniform_loc;

    public:
        glsl_big_uint_t(unsigned int shader_program, std::string uniform_string) {
            uniform_loc.size = glGetUniformLocation(shader_program, (uniform_string+".size").c_str());
            uniform_loc.table = glGetUniformLocation(shader_program, (uniform_string+".table").c_str());
        }
        
        void setUniform(const big_uint_t& val) const {
            glUniform1ui(uniform_loc.size, val.size);
            glUniform1uiv(uniform_loc.table, sizeof(val.table)/sizeof(val.table[0]), val.table);
        }
    };

    class glsl_big_float_t {
        glsl_big_uint_t exponent;
        glsl_big_uint_t mantissa;
        int info_loc;
    public:
        glsl_big_float_t(unsigned int shader_program, std::string uniform_string) : 
            exponent(shader_program, uniform_string+".exponent"),
            mantissa(shader_program, uniform_string+".mantissa") {
                glGetUniformLocation(shader_program, (uniform_string+".info").c_str());
            }
        
        void setUniform(const big_float_t& val) const {
            exponent.setUniform(val.exponent);
            mantissa.setUniform(val.mantissa);
            glUniform1ui(info_loc, val.info);
        }
    };

namespace my_window {
    constexpr size_t        height = 400;           // window height
    constexpr size_t        width  = 400;           // window width
    constexpr size_t        man_prec = 1;           // mantissa precision
    constexpr size_t        exp_prec = 2;           // exponent precision
    constexpr char const*   title  = "mandelbrot";  // window name
    constexpr float         start_offset_x =  0.2;  // starting x offset of mandelbrot
    constexpr float         start_offset_y =  0.0;  // starting y offset of mandelbrot
    constexpr float         start_zoom     =  2.0;  // starting zoom of mandelbrot
    constexpr float         zoom_step      =  0.2;  // how much a scroll movement scrolls in
    constexpr size_t        max_deque_size = 25;    // maximum amount of "back" clicks to remember
};

void handle_mouse(GLFWwindow* window);

// callback defines
void event_error_callback(int code, const char* description);
void event_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void event_mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void event_framebuffer_size_callback(GLFWwindow* window, int width, int height);
void event_scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

big_float_t offset_x;
big_float_t offset_y;
big_float_t zoom;
big_float_t zoom_lvl;
std::deque<big_float_t> prev_diff_x, prev_diff_y;

#define MAX_ITTERATIONS     10

void step_mandelbrot_big_num(
		big_float_t* z_r, big_float_t* z_i,
		big_float_t c_r, big_float_t c_i)
{
	big_float_t z_r_t, z_i_t, z_r_t2, z_i_t2;
	big_float_init_big_float(&z_r_t, my_window::man_prec, my_window::exp_prec, z_r);
	big_float_init_big_float(&z_i_t, my_window::man_prec, my_window::exp_prec, z_i);
	z_r_t2 = z_r_t; // copy here as mul does not work when both are the same object
    z_i_t2 = z_i_t; // copy here as mul does not work when both are the same object
    // calculate 'z.real
	big_float_mul(&z_r_t, &z_r_t2, true); 	// z_r_t = z.real^2
    big_float_mul(&z_i_t, &z_i_t2, true);	// z_i_t = z.imag^2
    big_float_sub(&z_r_t, &z_i_t, true);	// z_r_t = z.real^2 - z.imag^2
    big_float_add(&z_r_t, &c_r, true);	    // z_r_t = z.real^2 - z.imag^2 + c.real

	// calculate 'z.imag
	big_float_set_uint(&z_i_t, 2);
	big_float_mul(&z_i_t, z_r, true);	    // 2 * z.real
	big_float_mul(&z_i_t, z_i, true);	    // 2 * z.real * z.imag
	big_float_add(&z_i_t, &c_i, true);	    // 2 * z.real * z.imag + c.imag

	big_float_set_big_float(z_r, &z_r_t);
	big_float_set_big_float(z_i, &z_i_t);
}

void step_mandelbrot(
    double* z_r_ptr, double* z_i_ptr,
    double c_r, double c_i)
{

    double z_r, z_i, z_r_t, z_i_t;
    z_r = *z_r_ptr;
    z_i = *z_i_ptr;

    z_r_t = z_r * z_r - z_i * z_i + c_r;
    z_i_t = 2 * z_r * z_i + c_i;

    *z_r_ptr = z_r_t;
    *z_i_ptr = z_i_t;
}

double mandelbrot_bignum(double c_x, double c_y, big_float_t offset_r, big_float_t offset_i, big_float_t zoom)
{
	big_float_t c_r, c_i, z_r, z_i;
	// C = (x, y)
	big_float_init_double(&c_r, my_window::man_prec, my_window::exp_prec, c_x);
	big_float_init_double(&c_i, my_window::man_prec, my_window::exp_prec, c_y);
	big_float_init(&z_r, my_window::man_prec, my_window::exp_prec);
	big_float_init(&z_i, my_window::man_prec, my_window::exp_prec);
	// Z = (0, 0)
	big_float_set_zero(&z_r);
	big_float_set_zero(&z_i);

	// apply translation
    big_float_init(&zoom, my_window::man_prec, my_window::exp_prec);
	big_float_set_double(&zoom, pow(2, 2));
	big_float_mul(&c_r, &zoom, true);
	big_float_mul(&c_i, &zoom, true);
	
	big_float_sub(&c_r, &offset_r, true);
	big_float_sub(&c_i, &offset_i, true);

	int itterations = 0;
	for (; itterations <= MAX_ITTERATIONS; itterations++) {
		big_float_t a_sqr, b_sqr, four;
		big_float_init_big_float(&a_sqr, my_window::man_prec, my_window::exp_prec, &z_r);
		big_float_init_big_float(&b_sqr, my_window::man_prec, my_window::exp_prec, &z_i);
        big_float_init_uint(&four, my_window::man_prec, my_window::exp_prec, 4U);
		big_float_mul(&a_sqr, &z_r, true); // a^2 = z_r^2
        big_float_mul(&b_sqr, &z_i, true); // b^2 = z_i^2
        big_float_add(&a_sqr, &b_sqr, true); // a^2 + b^2 = c^2
        
        if (big_float_cmp_bigger(&a_sqr, &four)) { // pretend a_sqr here is the length of z^2
            return (double)itterations;
        }

		step_mandelbrot_big_num(&z_r, &z_i, c_r, c_i);
	}

	return INFINITY;
}

double mandelbrot(double c_x, double c_y, double offset_r, double offset_i, double zoom)
{
    double c_r = c_x, c_i = c_y, z_r = 0, z_i = 0;
    c_r *= zoom;
    c_i *= zoom;
    c_r -= offset_r;
    c_i -= offset_i;

    int itterations = 0;
    for (; itterations <= MAX_ITTERATIONS; itterations++) {
        if ((z_r*z_r + z_i*z_i) > 4.0) {
            return (double)itterations;
        }
        step_mandelbrot(&z_r, &z_i, c_r, c_i);
    }

    return INFINITY;
}

// profiling
void countFPS();

int main(void)
{
    big_float_init_float(&offset_x, my_window::man_prec, my_window::exp_prec, my_window::start_offset_x);
    big_float_init_float(&offset_y, my_window::man_prec, my_window::exp_prec, my_window::start_offset_y);
    big_float_init_double(&zoom, my_window::man_prec, my_window::exp_prec, pow(2, my_window::start_zoom));
    big_float_init_float(&zoom_lvl, my_window::man_prec, my_window::exp_prec, my_window::start_zoom);

    GLFWwindow* window;

    // shader build output buffer
    int success;
    char infoLog[512];

    std::cout << "Compiled against GLFW " 
        << GLFW_VERSION_MAJOR << "." << GLFW_VERSION_MINOR << "." << GLFW_VERSION_REVISION << std::endl;
    #ifdef DEBUG
        std::cout << "[DEBUG BUILD]" << std::endl;
        // #define TEST_ARB_PREC
        #ifdef TEST_ARB_PREC

        std::ofstream out_file;
        out_file.open("brot.txt", std::ios_base::out);
        for (size_t x = 0; x < my_window::width; x++) {
            for (size_t y = 0; y < my_window::height; y++) {
                double c_x = (double)x / (double)my_window::width - 0.5;
                double c_y = (double)y / (double)my_window::height - 0.5;


                // double ittr = mandelbrot_bignum(c_x, c_y, offset_x, offset_y, zoom);
                double ittr = mandelbrot(c_x, c_y, my_window::start_offset_x, my_window::start_offset_y, pow(2, my_window::start_zoom));
                if (ittr != INFINITY)
                    out_file << "0";
                else
                    out_file << " ";

                // if (ittr != ittr_bignum) {
                //     std::cout << "mismatch calculation @ (" << c_x - my_window::start_offset_x << ", " << c_y - my_window::start_offset_y << ") ittr_bignum: " << ittr_bignum << ", ittr: " << ittr << std::endl;
                //     return 0;
                // }
            }
            out_file << "\n";
        }
        out_file.close();

        std::cout << "no mismatches found!" << std::endl;

        return 0;
        #endif
    #endif // DEBUG

    // Initialize the library
    if (!glfwInit()) {
        const char* description;
        int err = glfwGetError(&description);
        event_error_callback(err, description);
        std::cout << "[GLFW] [ERR]: Failed initialization" << std::endl;
        return -1;
    }

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
        const char* description;
        int err = glfwGetError(&description);
        event_error_callback(err, description);
        std::cout << "[GLFW] [ERR]: Failed to create window" << std::endl;
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

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success); // check compile output
    if(!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "[GL] [ERR]: \"Failed to compile fragment shader\", " << infoLog << std::endl;
        glfwTerminate();
        return -1;
    }

    // create biguint shader & compile
    unsigned int bigNumShader;
    bigNumShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(bigNumShader, 1, &GSV::bignum, NULL);
    glCompileShader(bigNumShader);

    glGetShaderiv(bigNumShader, GL_COMPILE_STATUS, &success); // check compile output
    if(!success) {
        glGetShaderInfoLog(bigNumShader, 512, NULL, infoLog);
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
    glAttachShader(shaderProgram, bigNumShader);
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
    glsl_big_float_t u_offset_r(shaderProgram, GSV::u_offset_r);
    glsl_big_float_t u_offset_i(shaderProgram, GSV::u_offset_i);
    glsl_big_float_t u_zoom(shaderProgram, GSV::u_zoom);

    glUseProgram(shaderProgram);        // use our shader for the triangle
    glBindVertexArray(VAO);             // use our rectangle VAO
    glUniform1f(u_time_loc, timeValue);
    glUniform2f(u_resolution_loc, (float)my_window::width, (float)my_window::height);
    u_offset_r.setUniform(offset_x);
    u_offset_i.setUniform(offset_y);
    u_zoom.setUniform(zoom);

    // Loop until the user closes the window
    while (!glfwWindowShouldClose(window)) {
        // Render here
        glClearColor(0.2f, 0.0f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        countFPS();

        // imagine having multiple of these for now    
        glUseProgram(shaderProgram);        // use our shader for the triangle
        glBindVertexArray(VAO);             // use our rectangle VAO
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); // draw the actual rectangle ( interpret the VAO as a triangle )

        timeValue = glfwGetTime();
        glUniform1f(u_time_loc, timeValue);
        u_offset_r.setUniform(offset_x);
        u_offset_i.setUniform(offset_y);
        u_zoom.setUniform(zoom);

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

void handle_mouse(GLFWwindow* window)
{
    // prev_zoom.push(zoom);
    prev_diff_x.push_front(offset_x);
    prev_diff_y.push_front(offset_y);
    if (prev_diff_x.size() > my_window::max_deque_size)
        prev_diff_x.pop_back();
    if (prev_diff_y.size() > my_window::max_deque_size)
        prev_diff_y.pop_back();


    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    // translate coordinates to center
    big_float_t diff_x, diff_y;
    big_float_init_double(&diff_x, my_window::man_prec, my_window::exp_prec, (xpos - my_window::width /2) / (my_window::width /2));
    big_float_init_double(&diff_y, my_window::man_prec, my_window::exp_prec, (ypos - my_window::height/2) / (my_window::height/2));

    // divide zoom constant by 2 as number range is -1.0 - 1.0
    big_float_t two;
    big_float_init_uint(&two, my_window::man_prec, my_window::exp_prec, 2);
    big_float_t _zoom = zoom;
    big_float_div(&_zoom, &two, true);
    big_float_mul(&diff_x, &zoom, true);
    big_float_mul(&diff_y, &zoom, true);

    big_float_sub(&offset_x, &diff_x, true);
    big_float_add(&offset_y, &diff_y, true);

    std::cout << "zoom: 2^" << zoom_lvl << " = " << zoom 
        << "\ndiff (" << diff_x  << ", " << diff_y << ")" 
        << "\noffset: (" << offset_x << ", " << offset_y << ")" 
        << std::endl;
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
    PARAM_UNUSED(mods);
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
        handle_mouse(window);
    
    if (button == GLFW_MOUSE_BUTTON_4 && action == GLFW_PRESS) {
        if (prev_diff_x.size() > 0) {
            offset_x = prev_diff_x.front();
            offset_y = prev_diff_y.front();
            // zoom = prev_zoom.top();

            prev_diff_x.pop_front();
            prev_diff_y.pop_front();
            // prev_zoom.pop();
        }
    }
}

void event_scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    PARAM_UNUSED(window);
    PARAM_UNUSED(xoffset);

    big_float_t _yoffset;
    
    big_float_init_float(&_yoffset, my_window::man_prec, my_window::exp_prec, yoffset * my_window::zoom_step);
    big_float_sub(&zoom_lvl, &_yoffset, true);
    big_float_pow_big_frac(&zoom, &zoom_lvl);
    
    std::cout << "zoom: 2^" << zoom_lvl << " = " << zoom << std::endl;
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
 