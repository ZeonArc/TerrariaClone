#include "TextureAtlas.h"

TextureAtlas::TextureAtlas(int atlasWidth, int atlasHeight, int tileWidth, int tileHeight)
    : m_atlasWidth(atlasWidth), m_atlasHeight(atlasHeight), m_tileWidth(tileWidth), m_tileHeight(tileHeight) {
}

std::array<float, 8> TextureAtlas::getUVs(int col, int row) const {
    // Calculate pixel boundaries
    float x = (float)(col * m_tileWidth);
    float y = (float)(row * m_tileHeight);

    // U axis (Left to Right) is straightforward
    float uMin = x / m_atlasWidth;
    float uMax = (x + m_tileWidth) / m_atlasWidth;

    // V axis (Top to Bottom) is inverted because stbi_set_flip_vertically_on_load(true)
    // puts the top of the PNG at V = 1.0
    float vMax = 1.0f - (y / m_atlasHeight);                           // Top edge of the tile
    float vMin = 1.0f - ((y + m_tileHeight) / m_atlasHeight);          // Bottom edge of the tile

    return {
        uMax, vMax, // Top Right UV
        uMax, vMin, // Bottom Right UV
        uMin, vMin, // Bottom Left UV
        uMin, vMax  // Top Left UV
    };
}