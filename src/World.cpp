#include "World.h"
#include <cmath>

World::World(int seed, TextureAtlas atlas)
    : m_seed(seed), m_atlas(atlas), m_renderDistance(2) {
} // 2 chunks left and right

void World::update(const glm::vec2& playerPos) {
    // 1. Find which chunk the player is standing in
    int playerChunkX = (int)std::floor(playerPos.x / (CHUNK_WIDTH * TILE_SIZE));

    // 2. Generate missing chunks within the render distance
    for (int i = -m_renderDistance; i <= m_renderDistance; ++i) {
        int targetX = playerChunkX + i;

        // If the chunk doesn't exist in our map, create it!
        if (m_chunks.find(targetX) == m_chunks.end()) {
            m_chunks[targetX] = std::make_unique<Chunk>(targetX, 0, m_seed);
            m_chunks[targetX]->generateMesh(m_atlas);
        }
    }

    // 3. Unload chunks that are too far away (Garbage Collection)
    for (auto it = m_chunks.begin(); it != m_chunks.end(); ) {
        if (std::abs(it->first - playerChunkX) > m_renderDistance + 1) {
            it = m_chunks.erase(it); // Free the memory!
        }
        else {
            ++it;
        }
    }
}

void World::draw() const {
    for (const auto& pair : m_chunks) {
        pair.second->draw();
    }
}

uint16_t World::getTile(int worldGridX, int worldGridY) const {
    int chunkX = (int)std::floor((float)worldGridX / CHUNK_WIDTH);
    // Safe modulo math to ensure localX is always 0 to 31, even in negative chunks
    int localX = ((worldGridX % CHUNK_WIDTH) + CHUNK_WIDTH) % CHUNK_WIDTH;

    auto it = m_chunks.find(chunkX);
    if (it != m_chunks.end()) {
        return it->second->getTile(localX, worldGridY);
    }
    return 0; // Return Air if the chunk isn't loaded
}

void World::setTile(int worldGridX, int worldGridY, uint16_t id) {
    int chunkX = (int)std::floor((float)worldGridX / CHUNK_WIDTH);
    int localX = ((worldGridX % CHUNK_WIDTH) + CHUNK_WIDTH) % CHUNK_WIDTH;

    auto it = m_chunks.find(chunkX);
    if (it != m_chunks.end()) {
        if (it->second->getTile(localX, worldGridY) != id) {
            it->second->setTile(localX, worldGridY, id);
            it->second->generateMesh(m_atlas); // Rebuild mesh instantly
        }
    }
}