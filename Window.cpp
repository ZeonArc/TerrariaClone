#include "Window.h"
#include <iostream>

Window* Window::s_instance = nullptr;

Window::Window(int width, int height, const std::string& title)
    : m_width(width), m_height(height)
{
    s_instance = this;

    glfwSetErrorCallback(glfwErrorCallback);

    if (!glfwInit())
        throw std::runtime_error("Failed to initialise GLFW");

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    m_window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (!m_window) {
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window");
    }

    glfwMakeContextCurrent(m_window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        throw std::runtime_error("Failed to initialise GLAD");

    // Enable vsync
    glfwSwapInterval(1);

    glViewport(0, 0, m_width, m_height);
    glfwSetFramebufferSizeCallback(m_window, framebufferSizeCallback);

    // Print GPU info
    std::cout << "OpenGL Renderer : " << glGetString(GL_RENDERER) << "\n";
    std::cout << "OpenGL Version  : " << glGetString(GL_VERSION)  << "\n";
}

Window::~Window() {
    if (m_window) glfwDestroyWindow(m_window);
    glfwTerminate();
}

void Window::framebufferSizeCallback(GLFWwindow* /*win*/, int w, int h) {
    glViewport(0, 0, w, h);
    if (s_instance) {
        s_instance->m_width  = w;
        s_instance->m_height = h;
        if (s_instance->m_resizeCb)
            s_instance->m_resizeCb(w, h);
    }
}

void Window::glfwErrorCallback(int error, const char* desc) {
    std::cerr << "GLFW Error [" << error << "]: " << desc << "\n";
}
