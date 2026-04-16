#pragma once
#include <glad/glad.h>
#include <string>

class Texture {
public:
    // Loads the image file and generates an OpenGL texture
    Texture(const std::string& path);
    ~Texture();

    // Prevent copying to avoid double-freeing the OpenGL Texture ID
    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;

    // Bind the texture to a specific texture slot (default is 0)
    void bind(unsigned int slot = 0) const;
    void unbind() const;

    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }

    unsigned int ID;

private:
    int m_width;
    int m_height;
    int m_nrChannels;
};