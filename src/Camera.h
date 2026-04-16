#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "InputManager.h"

class Camera {
public:
    Camera(int viewportWidth, int viewportHeight);

    // Moves the camera based on keyboard input
    void update(float dt, const InputManager& input);
    void setPosition(const glm::vec2& pos) { m_position = pos; }

    // Updates the projection math if the user resizes the window
    void onResize(int width, int height);

    // The math that tells the GPU how to offset the world
    glm::mat4 getViewMatrix() const;
    glm::mat4 getProjectionMatrix() const;

    glm::vec2 getPosition() const { return m_position; }

private:
    glm::vec2 m_position;
    float m_viewportWidth;
    float m_viewportHeight;
    float m_moveSpeed;
};