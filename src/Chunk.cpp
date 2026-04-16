#include "Chunk.h"
#include "FastNoiseLite.h"
#include <iostream>

Chunk::Chunk(int chunkX, int chunkY, int seed) // <-- Update definition
    : m_chunkX(chunkX), m_chunkY(chunkY), m_indexCount(0)
{
    m_tiles.resize(CHUNK_WIDTH * CHUNK_HEIGHT, 0);

    // --- 1. Refined Surface Noise (FBM Terrain) ---
    FastNoiseLite terrainNoise;
    terrainNoise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    terrainNoise.SetFractalType(FastNoiseLite::FractalType_FBm);
    terrainNoise.SetFractalOctaves(4);        // 4 layers of detail
    terrainNoise.SetFractalLacunarity(2.0f);  // Each layer is twice as detailed as the last
    terrainNoise.SetFractalGain(0.5f);        // Each layer has half the impact on the height

    terrainNoise.SetFrequency(0.015f); // Stretched out for larger biomes
    terrainNoise.SetSeed(seed);

    // --- 2. Refined Cave Noise (Ridged Tunnels) ---
    FastNoiseLite caveNoise;
    caveNoise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);

    // "Ridged" noise is an industry trick. Instead of round blobs, it creates intersecting veins!
    caveNoise.SetFractalType(FastNoiseLite::FractalType_Ridged);
    caveNoise.SetFractalOctaves(2);
    caveNoise.SetFrequency(0.04f);
    caveNoise.SetSeed(seed);

    // --- 3. Generate the Terrain ---
    for (int x = 0; x < CHUNK_WIDTH; ++x) {
        float worldX = (float)(m_chunkX * CHUNK_WIDTH + x);

        // Calculate Surface Height
        // Noise returns -1.0 to 1.0. 
        // Multiply by 15.0f for massive 15-block high elevation changes!
        // Add 10 to establish the baseline ground level.
        int surfaceY = 10 + (int)(terrainNoise.GetNoise(worldX, 0.0f) * 15.0f);

        for (int y = 0; y < CHUNK_HEIGHT; ++y) {
            float worldY = (float)(m_chunkY * CHUNK_HEIGHT + y);

            // Calculate Caves
            bool isCave = false;
            // Only carve caves below the surface layer
            if (y > surfaceY + 2) {
                // Ridged noise behaves a bit differently, looking for values near 0 creates the best tunnels
                float caveVal = caveNoise.GetNoise(worldX, worldY);
                if (caveVal < 0.3f && caveVal > -0.3f) {
                    isCave = true;
                }
            }

            // Apply Blocks based on depth and caves
            if (y < surfaceY || isCave) {
                setTile(x, y, 0); // Air
            }
            else if (y == surfaceY) {
                setTile(x, y, 1); // Grass
            }
            else if (y > surfaceY && y < surfaceY + (4 + rand() % 3)) {
                // Add a random variance to dirt depth (between 4 and 6 blocks deep) so it looks natural
                setTile(x, y, 2); // Dirt
            }
            else {
                setTile(x, y, 3); // Deep Stone
            }
        }
    }

    // Generate GPU buffers
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);
}

Chunk::~Chunk() {
    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(1, &m_VBO);
    glDeleteBuffers(1, &m_EBO);
}

void Chunk::setTile(int x, int y, uint16_t id) {
    if (x >= 0 && x < CHUNK_WIDTH && y >= 0 && y < CHUNK_HEIGHT) {
        m_tiles[y * CHUNK_WIDTH + x] = id;
    }
}

uint16_t Chunk::getTile(int x, int y) const {
    if (x >= 0 && x < CHUNK_WIDTH && y >= 0 && y < CHUNK_HEIGHT) {
        return m_tiles[y * CHUNK_WIDTH + x];
    }
    return 0; // Return air if out of bounds
}

void Chunk::generateMesh(const TextureAtlas& atlas) {
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    unsigned int indexOffset = 0;

    for (int y = 0; y < CHUNK_HEIGHT; ++y) {
        for (int x = 0; x < CHUNK_WIDTH; ++x) {
            uint16_t id = getTile(x, y);
            if (id == 0) continue; // Skip air

            // Map block ID to Sprite Sheet coordinates (Col, Row)
            // Looking at your provided tileset.png:
// --- Map Block ID to Sprite Sheet (Col, Row) ---
            int atlasCol = 0, atlasRow = 0;

            // NOTE: You will need to adjust these numbers by looking at your image!
            // Since tiles are half the size, the column/row numbers will generally double.
            if (id == 1) { atlasCol = 14; atlasRow = 0; } // 1 = Dirt
            if (id == 2) { atlasCol = 12; atlasRow = 0; } // 2 = Grass
            if (id == 3) { atlasCol = 12; atlasRow = 11; } // 3 = Stone
            if (id == 4) { atlasCol = 14; atlasRow = 31; }

            std::array<float, 8> uvs = atlas.getUVs(atlasCol, atlasRow);

            // Calculate exact world position for this block
            float posX = (m_chunkX * CHUNK_WIDTH + x) * TILE_SIZE;
            float posY = (m_chunkY * CHUNK_HEIGHT + y) * TILE_SIZE;

            // --- Push 4 Vertices (X, Y, U, V) ---
            // Top Right
            vertices.push_back(posX + TILE_SIZE); vertices.push_back(posY);
            vertices.push_back(uvs[0]);           vertices.push_back(uvs[1]);
            // Bottom Right
            vertices.push_back(posX + TILE_SIZE); vertices.push_back(posY + TILE_SIZE);
            vertices.push_back(uvs[2]);           vertices.push_back(uvs[3]);
            // Bottom Left
            vertices.push_back(posX);             vertices.push_back(posY + TILE_SIZE);
            vertices.push_back(uvs[4]);           vertices.push_back(uvs[5]);
            // Top Left
            vertices.push_back(posX);             vertices.push_back(posY);
            vertices.push_back(uvs[6]);           vertices.push_back(uvs[7]);

            // --- Push 6 Indices (Two Triangles per block) ---
            indices.push_back(indexOffset + 0);
            indices.push_back(indexOffset + 1);
            indices.push_back(indexOffset + 3);
            indices.push_back(indexOffset + 1);
            indices.push_back(indexOffset + 2);
            indices.push_back(indexOffset + 3);

            indexOffset += 4; // Move forward 4 vertices for the next block
        }
    }

    m_indexCount = indices.size();

    // Upload the massive arrays to the GPU
    glBindVertexArray(m_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // Position (Location 0)
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // UVs (Location 1)
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void Chunk::draw() const {
    if (m_indexCount == 0) return; // Don't draw empty chunks!
    glBindVertexArray(m_VAO);
    glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}