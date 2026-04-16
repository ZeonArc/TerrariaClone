#pragma once
#include <map>
#include <memory>
#include <glm/glm.hpp>
#include "Chunk.h"
#include "TextureAtlas.h"

class World {
public:
    World(int seed, TextureAtlas atlas);

    // Generates chunks around the player and deletes far away ones
    void update(const glm::vec2& playerPos);

    // Tells the GPU to draw all loaded chunks
    void draw() const;

    // Global tile functions for Physics and Mining across chunk borders!
    uint16_t getTile(int worldGridX, int worldGridY) const;
    void setTile(int worldGridX, int worldGridY, uint16_t id);

private:
    int m_seed;
    int m_renderDistance;
    std::map<int, std::unique_ptr<Chunk>> m_chunks;
    TextureAtlas m_atlas;
};