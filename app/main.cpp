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

#include "gen_shaders.h"
#include "util.hpp"

namespace my_window {
    constexpr size_t        height = 800;           // window height
    constexpr size_t        width  = 800;           // window width
    constexpr char const*   title  = "mandelbrot";  // window name
    constexpr float         start_offset_x =  0.2;  // starting x offset of mandelbrot
    constexpr float         start_offset_y =  0.0;  // starting y offset of mandelbrot
    constexpr float         start_zoom     =  1.0;  // starting zoom of mandelbrot
    constexpr float         zoom_step      =  0.2;  // how much a scroll movement scrolls in
    constexpr size_t        max_deque_size = 25;    // maximum amount of "back" clicks to remember
};

// Arbitrary precision
// based on: https://github.com/RohanFredriksson/glsl-arbitrary-precision
class arb_prec_t {
    static constexpr int PRECISION = 5;
                                  
    static constexpr float BASE             = 4294967296.0f;
    static constexpr unsigned int HALF_BASE = 2147483648u;
    static constexpr unsigned int MASK      = 0xFFFF;

    typedef unsigned int arb_base_t;

    arb_base_t val[PRECISION+1];
public:
    arb_prec_t(void) : val{0} {}
    arb_prec_t(float val) {
        *this = val;
    }

    static constexpr size_t size() {
        return PRECISION+1;
    }

    static constexpr size_t precision() {
        return PRECISION;
    }

    unsigned int* buffer() {
        return &val[0];
    }

    arb_prec_t& zero(void) {
        // TODO: change to memset
        for(int zero_i = 0; zero_i <= PRECISION; zero_i++)
            this->val[zero_i] = 0u;
        return *this;
    }

    arb_prec_t& operator=(float load_value) {
        if (load_value == 0.0)
            return zero();

        this->val[0] = load_value < 0.0;
        load_value *= load_value < 0.0 ? -1.0 : 1.0;

        for(int load_i = 1; load_i <= PRECISION; load_i++) {
            this->val[load_i] = (unsigned int)(load_value); 
            load_value -= this->val[load_i];
            load_value *= BASE;
        }
        return *this;
    }

    arb_prec_t& shift(int shift_n) {
        for(int shift_i = shift_n+1; shift_i <= PRECISION; shift_i++)
            this->val[shift_i] = this->val[shift_i-shift_n];
        
        for(int shift_i = 1; shift_i<=shift_n; shift_i++)
            this->val[shift_i] = 0u;
        
        return *this;
    }

    arb_prec_t& negate(void) {
        this->val[0] = this->val[0]==0u ? 1u : 0u;
        return *this;
    }

    const arb_prec_t operator+(const arb_prec_t &b) {
        return arb_prec_t(*this) += b;
    }

    const arb_prec_t operator+(const float b) {
        return arb_prec_t(*this) += arb_prec_t(b);
    }

    const arb_prec_t operator-(const arb_prec_t &b) {
        return arb_prec_t(*this) -= b;
    }

    const arb_prec_t operator-(const float b) {
        return arb_prec_t(*this) -= arb_prec_t(b);
    }

    const arb_prec_t operator*(const arb_prec_t &b) {
        return arb_prec_t(*this) *= b;
    }

    const arb_prec_t operator*(const float b) {
        return arb_prec_t(*this) * arb_prec_t(b);
    }

    const arb_prec_t operator/(const float b) {
        return arb_prec_t(*this) /= b;
    }

    arb_prec_t& operator+=(const float b) {
        return *this += arb_prec_t(b);
    }

    arb_prec_t& operator-=(const float b) {
        return *this -= arb_prec_t(b);
    }

    arb_prec_t& operator*=(const float b) {
        return *this *= arb_prec_t(b);
    }

    arb_prec_t& operator/=(const float b) {
        return *this *= arb_prec_t(1/b);
    }

    arb_prec_t& operator-=(const arb_prec_t &b) {
        return *this += arb_prec_t(b).negate();
    }

    arb_prec_t& operator+=(const arb_prec_t &b) {
        arb_base_t add_buffer[PRECISION+1]; 
        bool add_pa = this->val[0] == 0u; 
        bool add_pb = b.val[0] == 0u; 
        
        if(add_pa == add_pb) {
            arb_base_t add_carry = 0u;

            for(int add_i=  PRECISION; add_i > 0; add_i--) {
                arb_base_t add_next = (arb_base_t)(this->val[add_i] + b.val[add_i] < this->val[add_i]);
                add_buffer[add_i] = this->val[add_i] + b.val[add_i] + add_carry;
                add_carry = add_next;
            }
            add_buffer[0] = (arb_base_t)(!add_pa);

        } else {
            bool add_flip=false; // true if b > a

            for(int add_i = 1; add_i <= PRECISION; add_i++) {
                if(b.val[add_i] > this->val[add_i]) {
                    add_flip=true; 
                    break;
                } 
                if(this->val[add_i] > b.val[add_i]) {
                    break;
                }
            }

            arb_base_t add_borrow = 0u;
            if(add_flip) { // do -1 * (b - a)
                for(int add_i = PRECISION; add_i > 0; add_i--) {
                    add_buffer[add_i] = b.val[add_i] - this->val[add_i] - add_borrow; 
                    add_borrow = (arb_base_t)(b.val[add_i] < this->val[add_i] + add_borrow);
                }
            } else { // do (a - b)
                for(int add_i = PRECISION; add_i > 0; add_i--) {
                    add_buffer[add_i] = this->val[add_i] - b.val[add_i] - add_borrow; 
                    add_borrow = (arb_base_t)(this->val[add_i] < b.val[add_i] || this->val[add_i] < b.val[add_i] + add_borrow);
                }
            }

            add_buffer[0] = (arb_base_t)(add_pa == add_flip);
        }

        for (int assign_i = 0; assign_i <= PRECISION; assign_i++)
            this->val[assign_i] = add_buffer[assign_i];
        
        return *this;
    }

    arb_prec_t& operator*=(const arb_prec_t &b) {
        arb_base_t mul_buffer[PRECISION+1] = {0};
        arb_base_t mul_product[2*PRECISION-1] = {0};

        for(int mul_i = 0; mul_i < PRECISION; mul_i++) {
            arb_base_t mul_carry = 0u; 
            for(int mul_j = 0; mul_j < PRECISION; mul_j++) {
                arb_base_t mul_next = 0; 
                arb_base_t mul_value = this->val[PRECISION-mul_i] * b.val[PRECISION-mul_j]; 
                if (mul_product[mul_i+mul_j] + mul_value < mul_product[mul_i+mul_j]) {
                    mul_next++;
                } 
                mul_product[mul_i+mul_j] += mul_value; 
                if(mul_product[mul_i+mul_j] + mul_carry < mul_product[mul_i+mul_j]) {
                    mul_next++;
                } 
                mul_product[mul_i+mul_j] += mul_carry; 
                arb_base_t mul_lower_a = this->val[PRECISION-mul_i] % (MASK + 1); 
                arb_base_t mul_upper_a = this->val[PRECISION-mul_i] / MASK; 
                arb_base_t mul_lower_b = b.val[PRECISION-mul_j] % (MASK + 1); 
                arb_base_t mul_upper_b = b.val[PRECISION-mul_j] / MASK;
                arb_base_t mul_lower = mul_lower_a * mul_lower_b; 
                arb_base_t mul_upper = mul_upper_a * mul_upper_b; 
                arb_base_t mul_mid = mul_lower_a * mul_upper_b; 
                mul_upper += mul_mid / MASK;
                mul_mid = mul_mid * MASK;

                if (mul_lower+mul_mid<mul_lower) {
                    mul_upper++;
                }

                mul_lower += mul_mid; 
                mul_mid = mul_lower_b * mul_upper_a;
                mul_upper += mul_mid / MASK;
                mul_mid = mul_mid * MASK;
                
                if(mul_lower + mul_mid < mul_lower) {
                    mul_upper++;
                }
                
                mul_carry = mul_upper + mul_next;
            }
            
            if(mul_i + PRECISION < 2*PRECISION-1) {
                mul_product[mul_i+PRECISION] += mul_carry;
            }
        }
        if(mul_product[PRECISION-2] >= HALF_BASE) {
            for(int mul_i = PRECISION-1; mul_i < 2*PRECISION-1; mul_i++) {
                if(mul_product[mul_i] + 1 > mul_product[mul_i]) {
                    mul_product[mul_i]++;
                    break;
                }
                mul_product[mul_i]++;
            }
        }
        for(int mul_i = 0; mul_i < PRECISION; mul_i++) {
            mul_buffer[mul_i+1] = mul_product[2*PRECISION-2-mul_i];
        } if((this->val[0] == 0u) != (b.val[0] == 0u)) {
            mul_buffer[0] = 1u;
        }
        
        for (int assign_i = 0; assign_i <= PRECISION; assign_i++)
            this->val[assign_i] = mul_buffer[assign_i];
        
        return *this;
    }

    friend std::ostream& operator<<(std::ostream& os, const arb_prec_t& dt) {
        if (dt.val[0])
            os << "-";
        
        unsigned int num_length = (unsigned int)ceil(log10(BASE));
        char* full_number = new char[num_length*arb_prec_t::size()-2];
        char* format_num = new char[num_length+2];
        double decimal_base = pow(10, num_length);
        sprintf(format_num, "%%0%u.%ulf", (num_length*2)+1, num_length);
        snprintf(full_number, num_length+2, format_num, (double)dt.val[1]);
        char* num_ptr = &full_number[num_length+1];
        for (unsigned int print_i = 2; print_i < arb_prec_t::size(); print_i++) {
            double base_mult = 1.0;
            for (unsigned int i = 0; i < print_i-1; i++) {
                base_mult *= 1.0 / (double)BASE;
                if (i > 0)
                    base_mult *= decimal_base;
            }
            // get normalized number=val*(pow(pow(10, num_length), (n-1))/pow(BASE, n))
            double normalized = base_mult * (double)(dt.val[print_i]);
            // check if normalized overflows in previous number, if so, add
            if ((unsigned int)normalized) {
                double last_num = atof(num_ptr-num_length);
                snprintf(num_ptr-num_length, num_length+1, format_num, normalized + last_num);
            }
            // get decimal part of current number
            double f, i;
            f = modf(normalized, &i);
            f *= decimal_base;
            snprintf(num_ptr, num_length+1, format_num, f);
            num_ptr+=num_length;
        }
        os << full_number;
        
        delete[] full_number;
        delete[] format_num;

        return os;
    }
};

void handle_mouse(GLFWwindow* window);

// callback defines
void event_error_callback(int code, const char* description);
void event_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void event_mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void event_framebuffer_size_callback(GLFWwindow* window, int width, int height);
void event_scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

arb_prec_t offset_x(my_window::start_offset_x);
arb_prec_t offset_y(my_window::start_offset_y);
arb_prec_t zoom(my_window::start_zoom);
float zoom_lvl = my_window::start_zoom;
std::deque<arb_prec_t> prev_diff_x, prev_diff_y;

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
    #ifdef DEBUG
        std::cout << "[DEBUG BUILD]" << std::endl;
    #endif // DEBUG

    for (int i = 0; i < fabs(floorf(zoom_lvl)); i++)
        zoom *= (zoom_lvl <= 0 ? 0.5 : 2.0);
    if ((zoom_lvl-floorf(zoom_lvl)) > 0.01)
        zoom *= powf(2.0f, -1*(zoom_lvl-floorf(zoom_lvl)));

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
    int u_offset_r_loc = glGetUniformLocation(shaderProgram, GSV::u_offset_r);
    int u_offset_i_loc = glGetUniformLocation(shaderProgram, GSV::u_offset_i);
    int u_zoom_loc = glGetUniformLocation(shaderProgram, GSV::u_zoom);

    glUseProgram(shaderProgram);        // use our shader for the triangle
    glBindVertexArray(VAO);             // use our rectangle VAO
    glUniform1f(u_time_loc, timeValue);
    glUniform2f(u_resolution_loc, (float)my_window::width, (float)my_window::height);
    glUniform1uiv(u_offset_r_loc, offset_x.size(), offset_x.buffer());
    glUniform1uiv(u_offset_i_loc, offset_y.size(), offset_y.buffer());
    glUniform1uiv(u_zoom_loc, zoom.size(), zoom.buffer());

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
        glUniform1uiv(u_offset_r_loc, offset_x.size(), offset_x.buffer());
        glUniform1uiv(u_offset_i_loc, offset_y.size(), offset_y.buffer());
        glUniform1uiv(u_zoom_loc, zoom.size(), zoom.buffer());

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
    arb_prec_t diff_x, diff_y;
    diff_x = (xpos - my_window::width /2) / (my_window::width /2);
    diff_y = (ypos - my_window::height/2) / (my_window::height/2);

    // divide zoom constant by 2 as number range is -1.0 - 1.0
    diff_x *= zoom / 2;
    diff_y *= zoom / 2;

    offset_x += diff_x.negate();
    offset_y += diff_y;

    std::cout << "zoom: 2^" << zoom_lvl << " = " << zoom
                << "\n diff (" << diff_x << ", " << diff_y << ")"
                << "\n offset: (" << offset_x << ", " << offset_y << ")" 
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

    if (yoffset > 0)
        zoom *= powf(2.0f, -my_window::zoom_step);
    else if (yoffset < 0)
        zoom *= powf(2.0f, my_window::zoom_step);
    
    zoom_lvl -= yoffset * 0.2;
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
 