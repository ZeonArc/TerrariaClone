#include "Renderer.h"
#include "TextureAtlas.h"
#include "World.h"
#include "Camera.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <ctime>

Renderer::Renderer() {}
Renderer::~Renderer() {}

void Renderer::init() {
    m_shader = std::make_unique<Shader>("shaders/tile.vert", "shaders/tile.frag");
    m_texture = std::make_unique<Texture>("assets/tileset.png");
    TextureAtlas atlas(m_texture->getWidth(), m_texture->getHeight(), 8, 8);// 8x8 grid

    // --- 1. Init Chunk ---
    m_world = std::make_unique<World>((int)std::time(nullptr), atlas);

    // --- 2. Init Player Graphic ---
    // Let's use a wooden plank (Column 10, Row 0 approx) as a placeholder player sprite
    std::array<float, 8> uvs = atlas.getUVs(10, 0);

    // A simple 1x1 square. We will stretch it using the Model matrix.
    float playerVertices[] = {
        // X     Y       U       V
        1.0f,  0.0f,   uvs[0], uvs[1], // top right
        1.0f,  1.0f,   uvs[2], uvs[3], // bottom right
        0.0f,  1.0f,   uvs[4], uvs[5], // bottom left
        0.0f,  0.0f,   uvs[6], uvs[7], // top left
        1.0f,  0.0f,   uvs[0], uvs[1], // top right
        0.0f,  1.0f,   uvs[4], uvs[5]  // bottom left
    };

    glGenVertexArrays(1, &m_playerVAO);
    glGenBuffers(1, &m_playerVBO);
    glBindVertexArray(m_playerVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_playerVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(playerVertices), playerVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);
}

void Renderer::draw(const Camera& camera, const Player& player) {
    if (m_world) {
        m_shader->use();
        m_texture->bind();

        m_shader->setMat4("projection", camera.getProjectionMatrix());
        m_shader->setMat4("view", camera.getViewMatrix());

        // --- DRAW THE CHUNK ---
        // The chunk already calculated its own world coordinates, so its Model matrix is just the Identity matrix (no change).
        glm::mat4 identity = glm::mat4(1.0f);
        m_shader->setMat4("model", identity);
        m_world->draw();

        // --- DRAW THE PLAYER ---
        // Calculate the player's exact bounding box using translation and scaling
        glm::mat4 playerModel = glm::mat4(1.0f);
        playerModel = glm::translate(playerModel, glm::vec3(player.getPosition(), 0.0f));
        playerModel = glm::scale(playerModel, glm::vec3(player.getSize(), 1.0f));

        m_shader->setMat4("model", playerModel);

        glBindVertexArray(m_playerVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
    }
}
