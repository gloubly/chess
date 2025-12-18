#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


class Renderer {
public:
	Renderer();

	~Renderer();

	void init();

	void renderTile();

	void renderSprite();

private:
	unsigned int tile_vao, tile_vbo, tile_ebo;
	unsigned int sprite_vao, sprite_vbo, sprite_ebo;
};