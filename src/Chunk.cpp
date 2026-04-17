#include "Chunk.h"
#include "FastNoiseLite.h"
#include <iostream>

Chunk::Chunk(int chunkX, int chunkY, int seed)
    : m_chunkX(chunkX), m_chunkY(chunkY), m_indexCount(0)
{
    m_tiles.resize(CHUNK_WIDTH * CHUNK_HEIGHT, 0);

    // --- 1. Surface Hills ---
    FastNoiseLite terrainNoise;
    terrainNoise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    terrainNoise.SetFractalType(FastNoiseLite::FractalType_FBm);
    terrainNoise.SetFractalOctaves(3);
    // LOWER frequency = wider, smoother, rolling hills (Terraria style)
    terrainNoise.SetFrequency(0.005f);
    terrainNoise.SetSeed(seed);

    // --- 2. Dirt/Stone Transition ---
    // Terraria has a jagged border where dirt meets stone. We use noise for this!
    FastNoiseLite dirtNoise;
    dirtNoise.SetNoiseType(FastNoiseLite::NoiseType_Cellular); // Cellular makes blocky, jagged transitions
    dirtNoise.SetFrequency(0.05f);
    dirtNoise.SetSeed(seed + 1); // Offset the seed so it doesn't match the surface

    // --- 3. Cave Tunnels ---
    FastNoiseLite caveNoise;
    caveNoise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    caveNoise.SetFractalType(FastNoiseLite::FractalType_Ridged);
    caveNoise.SetFractalOctaves(2);
    // LOWER frequency = wider, more distinct tunnel networks
    caveNoise.SetFrequency(0.015f);
    caveNoise.SetSeed(seed + 2);

    // --- 4. Generation Loop ---
    for (int x = 0; x < CHUNK_WIDTH; ++x) {
        float worldX = (float)(m_chunkX * CHUNK_WIDTH + x);

        // Base Surface Height (Multiply by 12 for the max hill height, add 15 for sky room)
        int surfaceY = 15 + (int)(terrainNoise.GetNoise(worldX, 0.0f) * 12.0f);

        // How deep does the dirt go before hitting solid stone? (Varies between 5 and 10 blocks)
        int dirtThickness = 5 + (int)((dirtNoise.GetNoise(worldX, 0.0f) + 1.0f) * 2.5f);
        int stoneY = surfaceY + dirtThickness;

        for (int y = 0; y < CHUNK_HEIGHT; ++y) {
            float worldY = (float)(m_chunkY * CHUNK_HEIGHT + y);

            bool isCave = false;

            // DEPTH MASK: We only carve caves if we are deep underground!
            // This protects the top layer of dirt from becoming floating islands.
            int depthBelowSurface = y - surfaceY;

            if (depthBelowSurface > 6) {
                float caveVal = caveNoise.GetNoise(worldX, worldY);

                // TIGHT THRESHOLD: Ridged noise creates lines near 0.0. 
                // By making this window smaller (-0.05 to 0.05), we get narrow, worm-like tunnels instead of massive voids.
                if (caveVal > -0.05f && caveVal < 0.05f) {
                    isCave = true;
                }
            }

            // Apply the blocks!
            if (y < surfaceY || isCave) {
                setTile(x, y, 0); // Air
            }
            else if (y == surfaceY) {
                setTile(x, y, 1); // Grass
            }
            else if (y < stoneY) {
                setTile(x, y, 2); // Dirt
            }
            else {
                setTile(x, y, 3); // Stone
            }
        }
    }

    // --- 5. FLORA PASS (Trees) ---
    // Seed the random number generator using the chunk's X coordinate 
    // so the trees always spawn in the exact same spots if you walk away and come back!
    srand(seed + m_chunkX * 100);

    // We start at x=2 and end at width-2 to prevent trees from clipping across chunk borders
    for (int x = 2; x < CHUNK_WIDTH - 2; ++x) {

        // 1. Scan downwards to find the surface Grass block
        int surfaceY = -1;
        for (int y = 0; y < CHUNK_HEIGHT; ++y) {
            if (getTile(x, y) == 1) { // 1 = Grass
                surfaceY = y;
                break;
            }
        }

        // 2. If we found a valid surface, roll the dice!
        if (surfaceY != -1) {
            // 1-in-8 chance to grow a tree on this specific block
            if (rand() % 8 == 0) {

                // Randomize tree height between 3 and 5 blocks
                int trunkHeight = 3 + (rand() % 3);

                // --- BUILD THE TRUNK ---
                for (int i = 1; i <= trunkHeight; ++i) {
                    setTile(x, surfaceY - i, 4); // 4 = Wood
                }

                // --- BUILD THE LEAVES ---
                int topY = surfaceY - trunkHeight;

                // A simple 3x3 box of leaves surrounding the top of the tree
                for (int leafX = -1; leafX <= 1; ++leafX) {
                    for (int leafY = -2; leafY <= 0; ++leafY) {
                        // Don't overwrite the wood trunk we just placed!
                        if (getTile(x + leafX, topY + leafY) == 0) {
                            setTile(x + leafX, topY + leafY, 5); // 5 = Leaves
                        }
                    }
                }

                // Skip the next 3 blocks so trees don't spawn inside each other
                x += 3;
            }
        }
    }

    // Generate GPU buffers
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);
    glGenVertexArrays(1, &m_VAO);
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
			if (id == 4) { atlasCol = 25; atlasRow = 31; } // 4 = Wood 
			if (id == 5) { atlasCol = 26; atlasRow = 21; } // 5 = Leaf

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