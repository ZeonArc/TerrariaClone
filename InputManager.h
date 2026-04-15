#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <array>
#include <functional>

class InputManager {
public:
    // Call once after window creation
    void init(GLFWwindow* window);

    // Call at the START of each frame (before pollEvents) to snapshot previous state
    void update();

    // Keyboard
    bool isKeyDown(int key)     const;   // held this frame
    bool isKeyPressed(int key)  const;   // went down this frame
    bool isKeyReleased(int key) const;   // went up  this frame

    // Mouse buttons
    bool isMouseDown(int button)     const;
    bool isMousePressed(int button)  const;
    bool isMouseReleased(int button) const;

    // Mouse position (screen coords, origin top-left)
    glm::vec2 getMousePos()   const { return m_mousePos; }
    glm::vec2 getMouseDelta() const { return m_mouseDelta; }
    float     getScrollDelta() const { return m_scrollDelta; }

private:
    static void keyCallback(GLFWwindow*, int key, int scancode, int action, int mods);
    static void mouseButtonCallback(GLFWwindow*, int button, int action, int mods);
    static void cursorPosCallback(GLFWwindow*, double x, double y);
    static void scrollCallback(GLFWwindow*, double xoff, double yoff);

    static InputManager* s_instance;

    static constexpr int KEY_COUNT    = GLFW_KEY_LAST + 1;
    static constexpr int BUTTON_COUNT = GLFW_MOUSE_BUTTON_LAST + 1;

    std::array<bool, KEY_COUNT>    m_keys     {};
    std::array<bool, KEY_COUNT>    m_keysPrev {};
    std::array<bool, BUTTON_COUNT> m_buttons     {};
    std::array<bool, BUTTON_COUNT> m_buttonsPrev {};

    glm::vec2 m_mousePos   {0.0f};
    glm::vec2 m_mousePosPrev {0.0f};
    glm::vec2 m_mouseDelta {0.0f};
    float     m_scrollDelta {0.0f};
    float     m_scrollAcc   {0.0f};
};
