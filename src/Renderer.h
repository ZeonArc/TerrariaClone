#pragma once
#include "Shader.h"
#include "Texture.h"
#include "Player.h"
#include "World.h"
#include "Camera.h"
#include <memory>


class Renderer {
public:
    Renderer();
    ~Renderer();
    void init();
    void draw(const Camera& camera, const Player& player);
    World* getWorld() { return m_world.get(); }

private:
    std::unique_ptr<Shader> m_shader;
    std::unique_ptr<Texture> m_texture;
    std::unique_ptr<World> m_world;
    unsigned int m_playerVAO, m_playerVBO;
};

