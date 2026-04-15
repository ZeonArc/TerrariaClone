#include "Game.h"
#include <glad/glad.h>
#include <iostream>
#include <algorithm>

Game::Game(int width, int height, const char* title)
    : m_window(width, height, title)
{
    m_input.init(m_window.getNative());

    m_window.setResizeCallback([this](int w, int h) {
        onResize(w, h);
    });

    // OpenGL global state
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Game::run() {
    double lastTime = glfwGetTime();

    while (!m_window.shouldClose()) {
        double now = glfwGetTime();
        float  dt  = (float)(now - lastTime);
        lastTime   = now;

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
            m_fpsTimer  -= 1.0f;
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
        std::cout << "Wireframe: " << (wireframe ? "ON" : "OFF") << "\n";
    }
}

void Game::update(float /*dt*/) {
    // Future: world.update(dt), player.update(dt), etc.
}

void Game::render() {
    // Sky blue clear — matches Terraria daytime sky
    glClearColor(0.478f, 0.749f, 0.969f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Future: renderer.drawWorld(), renderer.drawPlayer(), renderer.drawUI()
}

void Game::onResize(int w, int h) {
    // Future: camera.setViewport(w, h), renderer.onResize(w, h)
    (void)w; (void)h;
}
