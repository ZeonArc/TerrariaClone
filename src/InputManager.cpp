#include "InputManager.h"
#include <cassert>

InputManager* InputManager::s_instance = nullptr;

void InputManager::init(GLFWwindow* window) {
    s_instance = this;
    m_keys.fill(false);
    m_keysPrev.fill(false);
    m_buttons.fill(false);
    m_buttonsPrev.fill(false);

    // Register GLFW callbacks
    glfwSetKeyCallback(window, keyCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetCursorPosCallback(window, cursorPosCallback);
    glfwSetScrollCallback(window, scrollCallback);
}

void InputManager::update() {
    // Snapshot the current state into the "previous" state
    m_keysPrev = m_keys;
    m_buttonsPrev = m_buttons;

    // Calculate how far the mouse moved this frame
    m_mouseDelta = m_mousePos - m_mousePosPrev;
    m_mousePosPrev = m_mousePos;

    // Grab scroll wheel movement and reset the accumulator
    m_scrollDelta = m_scrollAcc;
    m_scrollAcc = 0.0f;
}

bool InputManager::isKeyDown(int key)      const { return m_keys[key]; }
bool InputManager::isKeyPressed(int key)   const { return  m_keys[key] && !m_keysPrev[key]; }
bool InputManager::isKeyReleased(int key)  const { return !m_keys[key] && m_keysPrev[key]; }

bool InputManager::isMouseDown(int b)      const { return m_buttons[b]; }
bool InputManager::isMousePressed(int b)   const { return  m_buttons[b] && !m_buttonsPrev[b]; }
bool InputManager::isMouseReleased(int b)  const { return !m_buttons[b] && m_buttonsPrev[b]; }

// ---------- static callbacks ----------

void InputManager::keyCallback(GLFWwindow*, int key, int /*sc*/, int action, int /*mods*/) {
    if (!s_instance || key < 0 || key >= KEY_COUNT) return;
    if (action == GLFW_PRESS)   s_instance->m_keys[key] = true;
    if (action == GLFW_RELEASE) s_instance->m_keys[key] = false;
}

void InputManager::mouseButtonCallback(GLFWwindow*, int button, int action, int /*mods*/) {
    if (!s_instance || button < 0 || button >= BUTTON_COUNT) return;
    if (action == GLFW_PRESS)   s_instance->m_buttons[button] = true;
    if (action == GLFW_RELEASE) s_instance->m_buttons[button] = false;
}

void InputManager::cursorPosCallback(GLFWwindow*, double x, double y) {
    if (!s_instance) return;
    s_instance->m_mousePos = { (float)x, (float)y };
}

void InputManager::scrollCallback(GLFWwindow*, double /*xoff*/, double yoff) {
    if (!s_instance) return;
    s_instance->m_scrollAcc += (float)yoff;
}