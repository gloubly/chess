#include "texture.hpp"

Texture::Texture() :
    wrap_s(GL_REPEAT), wrap_t(GL_REPEAT), min_filter(GL_NEAREST), mag_filter(GL_NEAREST)
{
    glGenTextures(1, &texture_id);
}

Texture::~Texture() {
    //glDeleteTextures(1, &this->texture_id);
}

void Texture::generate(unsigned int width, unsigned int height, unsigned int format, unsigned char* data) {
    bind();

    this->width = width;
    this->height = height;
    this->format = format;

    wrap_s = GL_CLAMP_TO_BORDER;
    wrap_t = GL_CLAMP_TO_BORDER;
    min_filter = GL_NEAREST;
    mag_filter = GL_NEAREST;

    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_s);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_t);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag_filter);
}

void Texture::blurrySettings() {
    bind();

    min_filter = GL_NEAREST;
    mag_filter = GL_LINEAR;

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag_filter);
}

Texture& Texture::bind() {
    glBindTexture(GL_TEXTURE_2D, texture_id);
    return *this;
}