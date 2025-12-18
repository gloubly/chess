#include "text_renderer.hpp"

TextRenderer::TextRenderer(const std::string& font_path) {
	FT_Library ft;
	if (FT_Init_FreeType(&ft)) {
		throw std::runtime_error("ERROR: could't init FreeType");
	}

	FT_Face face;
	if (FT_New_Face(ft, font_path.c_str(), 0, &face)) {
		throw std::runtime_error("ERROR: failed to load font: " + font_path);
	}

	FT_Set_Pixel_Sizes(face, 0, 48);

	if (FT_Load_Char(face, 'X', FT_LOAD_RENDER)) {
		throw std::runtime_error("ERROR: failed to load Glyph");
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	for (unsigned char c = 0; c < 128; ++c) {
		if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
			std::cout << "ERROR: failed to load Glyph '" << c << "'" << std::endl;
			continue;
		}

		//generate texture
		unsigned int texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			face->glyph->bitmap.width,
			face->glyph->bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			face->glyph->bitmap.buffer
		);

		// set texture options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// now store character for later use
		auto character = Character(
			texture,
			glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			static_cast<int>(face->glyph->advance.x >> 6) // advance is 1/64 pixels
			
		);
		characters.insert({ static_cast<char>(c), character });
	}
	line_height = static_cast<int>(face->size->metrics.height >> 6);
	ascender = static_cast<int>(face->size->metrics.ascender >> 6);
	descender = static_cast<int>(face->size->metrics.descender >> 6);

	FT_Done_Face(face);
	FT_Done_FreeType(ft);

	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
}

TextRenderer::~TextRenderer() {
	for (auto& [c, ch] : characters) {
		glDeleteTextures(1, &ch.texture_id);
	}
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
}

void TextRenderer::render(const std::string& text, Anchor anchor, std::optional<float> width, std::optional<float> height, TextAlign text_align) {
	if (text.length() == 0) return;
	if (!width.has_value() && !height.has_value()) {
		throw std::runtime_error("at least width or height must be defined");
	}
	if (width && *width <= 0) {
		throw std::runtime_error("invalid width value");
	} else if(height && *height <= 0) {
		throw std::runtime_error("invalid height value");
	}

	std::vector<std::string> lines = split(text, "\n");
	std::vector<int> lines_width;
	
	float scale = 0;
	int max_line_width = -1;

	for (std::string& line : lines) {
		int sum = 0;
		for (char& c : line) {
			auto it = characters.find(c);
			if (it == characters.end()) continue;
			sum += characters[c].advance;
		}
		lines_width.push_back(sum);
		if (sum > max_line_width) {
			max_line_width = sum;
		}
	}

	if (max_line_width == 0) {
		return;
	}

	float scale_x = width.value_or(0) / max_line_width;
	float scale_y = height.value_or(1) / ((lines.size() - 1) * line_height + ascender - descender);
	if (!width.has_value()) {
		scale = scale_y;
	}
	else if (!height.has_value()) {
		scale = scale_x;
	}
	else {
		scale = std::min(scale_x, scale_y); // keep aspect ratio
	}

	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(vao);

	float horizontal_factor = horizontalFactor(text_align.horizontal);
	float y = anchor.y + getYOffset(text_align.vertical, lines.size(), scale);

	for (int i = 0; i < lines.size(); ++i) {
		float x = anchor.x - horizontal_factor * lines_width[i] * scale;
		for (char& c : lines[i]) {
			auto it = characters.find(c);
			if (it == characters.end()) continue;
			const Character& ch = it->second;
			renderCharacter(ch, x, y, scale);
			x += ch.advance * scale;
		}
		y -= line_height * scale;
	}
}

void TextRenderer::render(const std::string& text, Anchor anchor, float scale, TextAlign text_align) {
	if (text.length() == 0) return;

	std::vector<std::string> lines = split(text, "\n");

	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(vao);

	float horizontal_factor = horizontalFactor(text_align.horizontal);

	float y = anchor.y + getYOffset(text_align.vertical, lines.size(), scale);

	for (std::string& line : lines) {
		int sum_line_advance = 0;
		if (text_align.horizontal != TextHorizontalAlign::LEFT) {
			for (char& c : line) {
				auto it = characters.find(c);
				if (it != characters.end()) {
					sum_line_advance += it->second.advance;
				}
			}
		}

		float x = anchor.x - horizontal_factor * sum_line_advance * scale;
		for (char& c : line) {
			Character ch = characters[c];
			renderCharacter(ch, x, y, scale);
			x += ch.advance * scale;
		}
		y -= line_height * scale;
	}
}

void TextRenderer::renderCharacter(const Character& ch, float x, float y, float scale) {
	float ch_x = x + ch.bearing.x * scale;
	float ch_y = y - (ch.size.y - ch.bearing.y) * scale;

	float w = ch.size.x * scale;
	float h = ch.size.y * scale;

	float vertices[6][4] = {
		{ch_x,     ch_y + h, 0.0f, 0.0f},
		{ch_x,     ch_y,     0.0f, 1.0f},
		{ch_x + w, ch_y,     1.0f, 1.0f},

		{ch_x,     ch_y + h, 0.0f, 0.0f},
		{ch_x + w, ch_y,     1.0f, 1.0f},
		{ch_x + w, ch_y + h, 1.0f, 0.0f},
	};

	//render glyph texture over quad
	glBindTexture(GL_TEXTURE_2D, ch.texture_id);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

	glDrawArrays(GL_TRIANGLES, 0, 6);
}

float TextRenderer::horizontalFactor(TextHorizontalAlign h) {
	switch (h) {
		case TextHorizontalAlign::CENTER: return 0.5f;
		case TextHorizontalAlign::RIGHT: return 1.0f;
		default: return 0.0f;
	}
}

float TextRenderer::getYOffset(TextVerticalAlign v, int nb_lines, float scale) {
	float vertical_factor;
	float offset;
	float text_height = ascender - descender + (nb_lines - 1) * line_height;
	switch (v) {
		case TextVerticalAlign::CENTER:
			offset = (text_height * 0.5f - ascender);
			break;
		case TextVerticalAlign::BOTTOM:
			offset = (text_height - ascender);
			break;
		default: 
			offset = -ascender;
			break;
	}
	return offset * scale;
}

std::vector<std::string> split(const std::string& s, const std::string& delimiter) {
	if (delimiter.empty()) {
		throw std::invalid_argument("delimiter can't be empty");
	}
	std::vector<std::string> texts;
	size_t start = 0;
	size_t pos = 0;
	while ((pos = s.find(delimiter, start)) != std::string::npos) {
		texts.push_back(s.substr(start, pos - start));
		start = pos + delimiter.length();
	}
	texts.push_back(s.substr(start));
	return texts;
}