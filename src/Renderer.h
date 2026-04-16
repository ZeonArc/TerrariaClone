#pragma once
#include "Shader.h"
#include "Texture.h"
#include "Player.h"
#include "World.h"
#include "Camera.h"
#include "Inventory.h"
#include <memory>


class Renderer {
public:
    Renderer();
    ~Renderer();
    void init();
    void draw(const Camera& camera, const Player& player);
    void drawUI(const Inventory& inventory, int screenWidth, int screenHeight);
    World* getWorld() { return m_world.get(); }

private:
    std::unique_ptr<Shader> m_shader;
    std::unique_ptr<Texture> m_texture;
    std::unique_ptr<World> m_world;
    std::unique_ptr<Shader> m_uiShader;
    unsigned int m_uiVAO, m_uiVBO;
    unsigned int m_playerVAO, m_playerVBO;
};

