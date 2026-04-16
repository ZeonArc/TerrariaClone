#include "Camera.h"
#include <GLFW/glfw3.h> // For the GLFW_KEY macros

Camera::Camera(int width, int height)
    : m_position(0.0f, 0.0f),
    m_viewportWidth((float)width),
    m_viewportHeight((float)height),
    m_moveSpeed(500.0f) // 500 pixels per second
{
}

void Camera::update(float dt, const InputManager& input) {
    if (input.isKeyDown(GLFW_KEY_W)) m_position.y -= m_moveSpeed * dt;
    if (input.isKeyDown(GLFW_KEY_S)) m_position.y += m_moveSpeed * dt;
    if (input.isKeyDown(GLFW_KEY_A)) m_position.x -= m_moveSpeed * dt;
    if (input.isKeyDown(GLFW_KEY_D)) m_position.x += m_moveSpeed * dt;
}

void Camera::onResize(int width, int height) {
    m_viewportWidth = (float)width;
    m_viewportHeight = (float)height;
}

glm::mat4 Camera::getViewMatrix() const {
    // This moves the entire world in the OPPOSITE direction of the camera.
    // It also centers the camera's position exactly in the middle of the screen.
    return glm::translate(glm::mat4(1.0f), glm::vec3(
        -m_position.x + m_viewportWidth / 2.0f,
        -m_position.y + m_viewportHeight / 2.0f,
        0.0f
    ));
}

glm::mat4 Camera::getProjectionMatrix() const {
    // Standard 2D orthographic projection
    return glm::ortho(0.0f, m_viewportWidth, m_viewportHeight, 0.0f, -1.0f, 1.0f);
}