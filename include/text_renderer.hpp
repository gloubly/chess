#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H
#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <unordered_map>
#include <string>
#include <vector>
#include <iostream>
#include <stdexcept>
#include <algorithm>
#include <optional>

enum class TextHorizontalAlign {
	CENTER,
	LEFT,
	RIGHT
};

enum class TextVerticalAlign {
	CENTER,
	TOP,
	BOTTOM
};

struct TextAlign {
	TextHorizontalAlign horizontal;
	TextVerticalAlign vertical;
};

struct Anchor {
	float x, y;
};

struct Character {
	unsigned int texture_id;
	glm::ivec2 size;
	glm::ivec2 bearing;
	int advance;
};

class TextRenderer {
public:
	// TODO create destructor

	TextRenderer(const std::string& font_path);

	~TextRenderer();
	
	void render(const std::string& text, Anchor anchor, std::optional<float> width, std::optional<float> height, TextAlign text_align);

	void render(const std::string& text, Anchor anchor, float scale, TextAlign text_align);

private:
	void renderCharacter(const Character& ch, float x, float y, float scale);

	float horizontalFactor(TextHorizontalAlign h);
	
	float getYOffset(TextVerticalAlign v, int nb_lines, float scale);

	unsigned int vao, vbo;

	int line_height, ascender, descender; // descender is negative

	std::unordered_map<char, Character> characters;
};

std::vector<std::string> split(const std::string& s, const std::string& delimiter);