#pragma once
#include "Window.h"
#include "InputManager.h"
#include "Camera.h"
#include "Player.h"
#include "Renderer.h" // <-- NEW
#include <memory>

class Game {
public:
    Game(int width, int height, const char* title);
    ~Game() = default;

    // Non-copyable / non-movable
    Game(const Game&) = delete;
    Game& operator=(const Game&) = delete;

    void run();

    // Accessors for subsystems
    Window& getWindow() { return m_window; }
    InputManager& getInput() { return m_input; }

private:
    void processInput();
    void update(float dt);
    void render();
    void onResize(int w, int h);
    Camera m_camera;

    Window       m_window;
    InputManager m_input;
    Renderer     m_renderer; // <-- NEW
    Player m_player;

    // Timing
    float m_fixedStep = 1.0f / 60.0f;  // physics runs at 60 Hz
    float m_accumulator = 0.0f;
    float m_totalTime = 0.0f;

    // Debug
    int   m_fpsCounter = 0;
    float m_fpsTimer = 0.0f;
};