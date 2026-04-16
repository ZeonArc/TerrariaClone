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
    m_uiShader = std::make_unique<Shader>("shaders/tile.vert", "shaders/ui.frag");
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
    glGenVertexArrays(1, &m_uiVAO);
    glGenBuffers(1, &m_uiVBO);
    glBindVertexArray(m_uiVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_uiVBO);

    // Notice GL_DYNAMIC_DRAW! We will rewrite this tiny buffer constantly every frame.
    glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);

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

void Renderer::drawUI(const Inventory& inventory, int screenWidth, int screenHeight) {
    // A 1:1 Pixel Projection Matrix. Top-Left is (0,0), Bottom-Right is (1280, 720).
    glm::mat4 projection = glm::ortho(0.0f, (float)screenWidth, (float)screenHeight, 0.0f, -1.0f, 1.0f);
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 model = glm::mat4(1.0f);

    float slotSize = 50.0f;
    float padding = 10.0f;
    float totalWidth = (9 * slotSize) + (8 * padding);
    float startX = (screenWidth - totalWidth) / 2.0f; // Center horizontally
    float startY = screenHeight - slotSize - 20.0f;   // 20 pixels from bottom

    TextureAtlas atlas(m_texture->getWidth(), m_texture->getHeight(), 8, 8);
    glBindVertexArray(m_uiVAO);

    // A fast helper function to update our Dynamic VBO and draw a square
    auto drawQuad = [&](float x, float y, float w, float h, std::array<float, 8> uvs) {
        float vertices[24] = {
            x + w, y,   uvs[0], uvs[1], // Top Right
            x + w, y + h, uvs[2], uvs[3], // Bottom Right
            x,   y + h, uvs[4], uvs[5], // Bottom Left
            x,   y,   uvs[6], uvs[7], // Top Left
            x + w, y,   uvs[0], uvs[1], // Top Right
            x,   y + h, uvs[4], uvs[5]  // Bottom Left
        };
        glBindBuffer(GL_ARRAY_BUFFER, m_uiVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        };

    std::array<float, 8> dummyUVs = { 0,0,0,0,0,0,0,0 }; // UVs don't matter for solid colors

    // Draw the 9 slots
    for (int i = 0; i < 9; ++i) {
        float x = startX + i * (slotSize + padding);

        // 1. Draw Selection Border (Yellow)
        if (i == inventory.getSelectedSlot()) {
            m_uiShader->use();
            m_uiShader->setMat4("projection", projection);
            m_uiShader->setMat4("view", view);
            m_uiShader->setMat4("model", model);
            m_uiShader->setVec4("color", glm::vec4(1.0f, 0.9f, 0.2f, 1.0f)); // Yellow

            drawQuad(x - 4, startY - 4, slotSize + 8, slotSize + 8, dummyUVs);
        }

        // 2. Draw Slot Background (Dark Gray)
        m_uiShader->use();
        m_uiShader->setMat4("projection", projection);
        m_uiShader->setMat4("view", view);
        m_uiShader->setMat4("model", model);
        m_uiShader->setVec4("color", glm::vec4(0.2f, 0.2f, 0.2f, 0.8f)); // Dark gray, slightly transparent

        drawQuad(x, startY, slotSize, slotSize, dummyUVs);

        // 3. Draw Item Icon inside the slot!
        const ItemStack& slot = inventory.getSlot(i);
        if (slot.blockID != 0) {
            m_shader->use(); // Switch back to the Texture Shader!
            m_texture->bind();
            m_shader->setMat4("projection", projection);
            m_shader->setMat4("view", view);
            m_shader->setMat4("model", model);

            // Map the Block ID to its image on the spritesheet
            int atlasCol = 0, atlasRow = 0;
            if (slot.blockID == 1) { atlasCol = 4; atlasRow = 0; } // Grass
            if (slot.blockID == 2) { atlasCol = 4; atlasRow = 2; } // Dirt
            if (slot.blockID == 3) { atlasCol = 4; atlasRow = 12; } // Stone
            if (slot.blockID == 4) { atlasCol = 10; atlasRow = 0; } // Wood

            std::array<float, 8> itemUVs = atlas.getUVs(atlasCol, atlasRow);

            // Draw it slightly smaller than the slot so it has a margin
            drawQuad(x + 10, startY + 10, slotSize - 20, slotSize - 20, itemUVs);
        }
    }
    glBindVertexArray(0);
}
