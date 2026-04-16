#pragma once
#include <array>

class TextureAtlas {
public:
    TextureAtlas(int atlasWidth, int atlasHeight, int tileWidth, int tileHeight);

    // Returns UV coordinates for a specific column and row (0-indexed, origin top-left)
    // Array Order: Top-Right, Bottom-Right, Bottom-Left, Top-Left (Matches your Renderer quad)
    std::array<float, 8> getUVs(int col, int row) const;

private:
    int m_atlasWidth;
    int m_atlasHeight;
    int m_tileWidth;
    int m_tileHeight;
};