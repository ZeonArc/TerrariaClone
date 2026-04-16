#include "Player.h"
#include "Chunk.h"
#include "World.h"
#include <algorithm>
#include <cmath>

Player::Player(glm::vec2 startPos)
    : m_position(startPos), m_velocity(0.0f), m_isGrounded(false)
{
    // A player is typically a bit narrower than a full tile, and slightly less than 2 tiles tall
    m_size = glm::vec2(TILE_SIZE * 0.8f, TILE_SIZE * 1.8f);
    m_speed = 250.0f;
    m_jumpForce = -450.0f; // Negative because Y points down in our orthographic projection
    m_gravity = 1200.0f;
}

void Player::update(float dt, const InputManager& input, const World& world) {
    // 1. Tighter Horizontal Input (A and D cancel each other out)
    float moveDir = 0.0f;
    if (input.isKeyDown(GLFW_KEY_A)) moveDir -= 1.0f;
    if (input.isKeyDown(GLFW_KEY_D)) moveDir += 1.0f;

    m_velocity.x = moveDir * m_speed;

    // 2. Snappier Vertical Input
    // Increase gravity to 2000.0f and JumpForce to -600.0f in your constructor if it still feels floaty!
    m_velocity.y += m_gravity * dt;
    if (m_isGrounded && input.isKeyPressed(GLFW_KEY_SPACE)) {
        m_velocity.y = m_jumpForce;
        m_isGrounded = false;
    }

    // 3. Move and Collide X
    m_position.x += m_velocity.x * dt;
    resolveCollisionX(world);

    // 4. Move and Collide Y
    m_position.y += m_velocity.y * dt;
    resolveCollisionY(world);
}

void Player::resolveCollisionX(const World& world) {
    // A tiny buffer to prevent snagging on the floor/ceiling when walking
    float epsilon = 0.1f;

    int startX = (int)std::floor(m_position.x / TILE_SIZE);
    int endX = (int)std::floor((m_position.x + m_size.x) / TILE_SIZE);

    // Apply the epsilon to the Y bounds during the X check!
    int startY = (int)std::floor((m_position.y + epsilon) / TILE_SIZE);
    int endY = (int)std::floor((m_position.y + m_size.y - epsilon) / TILE_SIZE);

    for (int y = startY; y <= endY; ++y) {
        for (int x = startX; x <= endX; ++x) {
            if (world.getTile(x, y) != 0) { // Solid block
                if (m_velocity.x > 0) {
                    m_position.x = x * TILE_SIZE - m_size.x; // Snap left
                }
                else if (m_velocity.x < 0) {
                    m_position.x = (x + 1) * TILE_SIZE;      // Snap right
                }
                m_velocity.x = 0;
                return;
            }
        }
    }
}

void Player::resolveCollisionY(const World& world) {
    float epsilon = 0.1f;

    // Apply the epsilon to the X bounds during the Y check to prevent wall-snagging!
    int startX = (int)std::floor((m_position.x + epsilon) / TILE_SIZE);
    int endX = (int)std::floor((m_position.x + m_size.x - epsilon) / TILE_SIZE);

    int startY = (int)std::floor(m_position.y / TILE_SIZE);
    int endY = (int)std::floor((m_position.y + m_size.y) / TILE_SIZE);

    m_isGrounded = false;

    for (int y = startY; y <= endY; ++y) {
        for (int x = startX; x <= endX; ++x) {
            if (world.getTile(x, y) != 0) {
                if (m_velocity.y > 0) { // Falling down
                    m_position.y = y * TILE_SIZE - m_size.y;
                    m_isGrounded = true;
                }
                else if (m_velocity.y < 0) { // Hitting head on ceiling
                    m_position.y = (y + 1) * TILE_SIZE;
                }
                m_velocity.y = 0;
                return;
            }
        }
    }
}