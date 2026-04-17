#include "Game.h"
#include "Camera.h"
#include "Player.h"
#include <glad/glad.h>
#include <iostream>
#include <algorithm>

Game::Game(int width, int height, const char* title)
    : m_window(width, height, title),
    m_camera(width, height),
    m_player(glm::vec2(100.0f, -100.0f)) // <-- NEW: Spawn player in the air
{
    m_input.init(m_window.getNative());

    m_window.setResizeCallback([this](int w, int h) {
        onResize(w, h);
        });

    // OpenGL global state
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Initialize the renderer and GPU assets AFTER OpenGL context exists
    m_renderer.init();
}

void Game::run() {
    double lastTime = glfwGetTime();

    while (!m_window.shouldClose()) {
        double now = glfwGetTime();
        float  dt = (float)(now - lastTime);
        lastTime = now;

        // Cap dt to avoid spiral-of-death on slow frames / debugger pauses
        dt = std::min(dt, 0.25f);

        m_totalTime += dt;

        // ---- Input (before pollEvents snapshot) ----
        m_input.update();
        m_window.pollEvents();
        processInput();

        // ---- Fixed-step physics accumulator ----
        m_accumulator += dt;
        while (m_accumulator >= m_fixedStep) {
            update(m_fixedStep);
            m_accumulator -= m_fixedStep;
        }

        // ---- Render ----
        render();
        m_window.swapBuffers();

        // ---- FPS counter ----
        m_fpsCounter++;
        m_fpsTimer += dt;
        if (m_fpsTimer >= 1.0f) {
            std::cout << "FPS: " << m_fpsCounter << "\n";
            m_fpsCounter = 0;
            m_fpsTimer -= 1.0f;
        }
    }
}

void Game::processInput() {
    if (m_input.isKeyPressed(GLFW_KEY_ESCAPE))
        glfwSetWindowShouldClose(m_window.getNative(), true);

    // Toggle wireframe with F1
    static bool wireframe = false;
    if (m_input.isKeyPressed(GLFW_KEY_F1)) {
        wireframe = !wireframe;
        glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);
    }

    // --- NEW: MINING (Left Click = 0/Air) ---
    if (m_input.isMouseDown(GLFW_MOUSE_BUTTON_LEFT)) {
        glm::vec2 mouse = m_input.getMousePos();
        float worldX = mouse.x + (m_camera.getPosition().x - 1280.0f / 2.0f);
        float worldY = mouse.y + (m_camera.getPosition().y - 720.0f / 2.0f);

        int gridX = (int)std::floor(worldX / 32.0f);
        int gridY = (int)std::floor(worldY / 32.0f);

        // What block are we clicking on?
        uint16_t targetedBlock = m_renderer.getWorld()->getTile(gridX, gridY);

        if (targetedBlock != 0) {
            // Give the item to the player!
            if (m_player.inventory.addItem(targetedBlock, 1)) {
                // If there was room in the inventory, destroy the block in the world
                m_renderer.getWorld()->setTile(gridX, gridY, 0);
            }
        }
    }
    // --- NEW: PLACING (Right Click = 4/Wood) ---
    if (m_input.isMouseDown(GLFW_MOUSE_BUTTON_RIGHT)) {
        glm::vec2 mouse = m_input.getMousePos();
        float worldX = mouse.x + (m_camera.getPosition().x - 1280.0f / 2.0f);
        float worldY = mouse.y + (m_camera.getPosition().y - 720.0f / 2.0f);

        int gridX = (int)std::floor(worldX / 32.0f);
        int gridY = (int)std::floor(worldY / 32.0f);

        // Check if we are clicking on empty air
        if (m_renderer.getWorld()->getTile(gridX, gridY) == 0) {

            // Do we have an item selected in our hotbar?
            uint16_t selectedBlock = m_player.inventory.getSelectedBlockID();

            if (selectedBlock != 0) {
                // Try to consume 1 item from the inventory
                if (m_player.inventory.consumeSelectedItem()) {
                    // Place it in the world!
                    m_renderer.getWorld()->setTile(gridX, gridY, selectedBlock);
                }
            }
        }
    }
    // --- HOTBAR SELECTION (Number Keys 1-9) ---
    if (m_input.isKeyPressed(GLFW_KEY_1)) m_player.inventory.setSelectedSlot(0);
    if (m_input.isKeyPressed(GLFW_KEY_2)) m_player.inventory.setSelectedSlot(1);
    if (m_input.isKeyPressed(GLFW_KEY_3)) m_player.inventory.setSelectedSlot(2);
    if(m_input.isKeyPressed(GLFW_KEY_4)) m_player.inventory.setSelectedSlot(3);
    if (m_input.isKeyPressed(GLFW_KEY_5)) m_player.inventory.setSelectedSlot(4);

}

void Game::update(float dt) {
    // 1. GENERATE THE WORLD FIRST
    // So the chunks exist before the player tries to stand on them!
    m_renderer.getWorld()->update(m_player.getPosition());

    // 2. UPDATE PLAYER PHYSICS SECOND
    m_player.update(dt, m_input, *m_renderer.getWorld());

    // 3. MOVE THE CAMERA LAST
    glm::vec2 playerCenter = m_player.getPosition() + (m_player.getSize() * 0.5f);
    m_camera.setPosition(playerCenter);
}
 
void Game::render() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Change to black!
    glClear(GL_COLOR_BUFFER_BIT);

    m_renderer.draw(m_camera, m_player);
    m_renderer.drawUI(m_player.inventory, 1280, 720);
}

void Game::onResize(int w, int h) {
    glViewport(0, 0, w, h);
    m_camera.onResize(w, h); // <-- NEW
}

