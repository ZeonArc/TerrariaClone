#pragma once
#include <vector>
#include <cstdint>
#include <glad/glad.h>
#include "TextureAtlas.h"

// Constants for our world grid
const int CHUNK_WIDTH = 32;
const int CHUNK_HEIGHT = 32;
const float TILE_SIZE = 32.0f; // Rendering them at 32x32 pixels so they are easy to see

class Chunk {
public:
    Chunk(int chunkX, int chunkY, int seed);
    ~Chunk();

    // Prevent copying because of OpenGL buffer IDs
    Chunk(const Chunk&) = delete;
    Chunk& operator=(const Chunk&) = delete;

    void setTile(int x, int y, uint16_t id);
    uint16_t getTile(int x, int y) const;

    // Iterates through all tiles and builds the Batch VBO
    void generateMesh(const TextureAtlas& atlas);
    void draw() const;

private:
    int m_chunkX;
    int m_chunkY;
    std::vector<uint16_t> m_tiles;

    // OpenGL Buffers
    unsigned int m_VAO, m_VBO, m_EBO;
    unsigned int m_indexCount;
};