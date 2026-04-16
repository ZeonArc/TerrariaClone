#pragma once
#include <glm/glm.hpp>
#include "World.h"
#include "InputManager.h"
#include "Inventory.h"


class Player {
public:
    Player(glm::vec2 startPos);
    Inventory inventory;

    void update(float dt, const InputManager& input, const World& world); // Update signature
    glm::vec2 getPosition() const { return m_position; }
    glm::vec2 getSize() const { return m_size; }

private:
    void resolveCollisionX(const World& world); // Update signature
    void resolveCollisionY(const World& world); // Update signature

    glm::vec2 m_position;
    glm::vec2 m_velocity;
    glm::vec2 m_size;

    float m_speed;
    float m_jumpForce;
    float m_gravity;
    bool m_isGrounded;
};