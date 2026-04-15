#pragma once
#include "Window.h"
#include "InputManager.h"
#include <memory>

class Game {
public:
    Game(int width, int height, const char* title);
    ~Game() = default;

    // Non-copyable / non-movable
    Game(const Game&) = delete;
    Game& operator=(const Game&) = delete;

    void run();

    // Accessors for subsystems (future systems will be added here)
    Window&       getWindow()  { return m_window; }
    InputManager& getInput()   { return m_input; }

private:
    void processInput();
    void update(float dt);
    void render();
    void onResize(int w, int h);

    Window       m_window;
    InputManager m_input;

    // Timing
    float m_fixedStep    = 1.0f / 60.0f;  // physics runs at 60 Hz
    float m_accumulator  = 0.0f;
    float m_totalTime    = 0.0f;

    // Debug
    int   m_fpsCounter   = 0;
    float m_fpsTimer     = 0.0f;
};
