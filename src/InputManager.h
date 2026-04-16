#pragma once
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <array>

class InputManager {
public:
    static const int KEY_COUNT = 1024;
    static const int BUTTON_COUNT = 8;

    void init(GLFWwindow* window);
    void update();

    // Keyboard
    bool isKeyDown(int key) const;
    bool isKeyPressed(int key) const;
    bool isKeyReleased(int key) const;

    // Mouse Buttons
    bool isMouseDown(int button) const;
    bool isMousePressed(int button) const;
    bool isMouseReleased(int button) const;

    // Mouse Positioning
    glm::vec2 getMousePos() const { return m_mousePos; }
    glm::vec2 getMouseDelta() const { return m_mouseDelta; }
    float getScrollDelta() const { return m_scrollDelta; }

private:
    static InputManager* s_instance;

    std::array<bool, KEY_COUNT> m_keys;
    std::array<bool, KEY_COUNT> m_keysPrev;

    std::array<bool, BUTTON_COUNT> m_buttons;
    std::array<bool, BUTTON_COUNT> m_buttonsPrev;

    glm::vec2 m_mousePos = { 0.0f, 0.0f };
    glm::vec2 m_mousePosPrev = { 0.0f, 0.0f };
    glm::vec2 m_mouseDelta = { 0.0f, 0.0f };

    float m_scrollAcc = 0.0f;
    float m_scrollDelta = 0.0f;

    // GLFW Callbacks
    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    static void cursorPosCallback(GLFWwindow* window, double xpos, double ypos);
    static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
};