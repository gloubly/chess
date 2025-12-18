#include "renderer.hpp"

Renderer::Renderer() {

}

Renderer::~Renderer() {
    glDeleteVertexArrays(1, &tile_vao);
    glDeleteBuffers(1, &tile_vbo);
    glDeleteBuffers(1, &tile_ebo);

    glDeleteVertexArrays(1, &sprite_vao);
    glDeleteBuffers(1, &sprite_vbo);
    glDeleteBuffers(1, &sprite_ebo);
}

void Renderer::init() {
    float square_positions[] = {
        0.5f, -0.5f, 0.0f, // bottom right
        0.5f,  0.5f, 0.0f, // top right
       -0.5f,  0.5f, 0.0f, // top left
       -0.5f, -0.5f, 0.0f, // bottom left
    };

    float square_texture[] = {
        1.0f, 0.0f, // bottom right
        1.0f, 1.0f, // top right
        0.0f, 1.0f, // top left
        0.0f, 0.0f, // bottom left
    };

    unsigned int square_indices[] = {
        0, 1, 2,
        0, 2, 3
    };

    // tile
    glGenVertexArrays(1, &tile_vao);
    glGenBuffers(1, &tile_vbo);
    glGenBuffers(1, &tile_ebo);

    glBindVertexArray(tile_vao);

    glBindBuffer(GL_ARRAY_BUFFER, tile_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(square_positions), square_positions, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, tile_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(square_indices), square_indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // sprite
    glGenVertexArrays(1, &sprite_vao);
    glGenBuffers(1, &sprite_vbo);
    glGenBuffers(1, &sprite_ebo);

    glBindVertexArray(sprite_vao);

    glBindBuffer(GL_ARRAY_BUFFER, sprite_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(square_positions) + sizeof(square_texture), nullptr, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(square_positions), square_positions);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(square_positions), sizeof(square_texture), square_texture);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sprite_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(square_indices), square_indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)(sizeof(square_positions)));
    glEnableVertexAttribArray(1);
}

void Renderer::renderTile() {
    glBindVertexArray(tile_vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void Renderer::renderSprite() {
    glBindVertexArray(sprite_vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}



