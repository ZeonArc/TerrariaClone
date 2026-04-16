#include "Texture.h"
#include <stb_image.h>
#include <iostream>

Texture::Texture(const std::string& path) {
    glGenTextures(1, &ID);
    glBindTexture(GL_TEXTURE_2D, ID);

    // Texture wrapping parameters (Repeat by default)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Texture filtering parameters (CRITICAL for 2D pixel art: GL_NEAREST)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // OpenGL expects the 0.0 coordinate on the Y-axis to be at the bottom, 
    // but image files usually have 0.0 at the top. This fixes it.
    stbi_set_flip_vertically_on_load(true);

    // Load image, create texture and generate mipmaps
    unsigned char* data = stbi_load(path.c_str(), &m_width, &m_height, &m_nrChannels, 0);
    if (data) {
        GLenum format = GL_RGBA; // Default to RGBA to avoid uninitialized memory errors
        if (m_nrChannels == 1)      format = GL_RED;
        else if (m_nrChannels == 3) format = GL_RGB;
        else if (m_nrChannels == 4) format = GL_RGBA;

        glTexImage2D(GL_TEXTURE_2D, 0, format, m_width, m_height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        std::cerr << "ERROR::TEXTURE::FAILED_TO_LOAD: " << path << std::endl;
    }

    // Free the image memory from RAM, it's safely on the GPU now
    stbi_image_free(data);
    glBindTexture(GL_TEXTURE_2D, 0);
}

Texture::~Texture() {
    glDeleteTextures(1, &ID);
}

void Texture::bind(unsigned int slot) const {
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, ID);
}

void Texture::unbind() const {
    glBindTexture(GL_TEXTURE_2D, 0);
}