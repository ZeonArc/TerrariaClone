#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <functional>
#include <stdexcept>

class Window {
public:
    Window(int width, int height, const std::string& title);
    ~Window();

    // Non-copyable
    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    bool shouldClose() const { return glfwWindowShouldClose(m_window); }
    void swapBuffers()        { glfwSwapBuffers(m_window); }
    void pollEvents()         { glfwPollEvents(); }

    int getWidth()  const { return m_width; }
    int getHeight() const { return m_height; }
    float getAspect() const { return (float)m_width / (float)m_height; }
    GLFWwindow* getNative() const { return m_window; }

    // Resize callback setter
    void setResizeCallback(std::function<void(int, int)> cb) { m_resizeCb = cb; }

private:
    static void framebufferSizeCallback(GLFWwindow* win, int w, int h);
    static void glfwErrorCallback(int error, const char* desc);

    GLFWwindow* m_window = nullptr;
    int m_width, m_height;
    std::function<void(int, int)> m_resizeCb;

    // Map GLFW window ptr -> this instance for static callbacks
    static Window* s_instance;
};
