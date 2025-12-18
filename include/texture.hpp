#include "stb_image.h"

#include <glad/glad.h>
#include <utility>
#include <vector>


class Texture {
public:
	Texture();

	~Texture();

	void generate(unsigned int width, unsigned int height, unsigned int format, unsigned char* data);

	void blurrySettings();

	Texture& bind();
	
	unsigned int wrap_s;
	unsigned int wrap_t;
	unsigned int min_filter;
	unsigned int mag_filter;

private:
	unsigned int texture_id;

	unsigned int width, height;

	unsigned int format;
};

