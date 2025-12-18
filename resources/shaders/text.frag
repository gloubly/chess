#version 460 core
in vec2 tex_coords;
out vec4 text_color;

uniform sampler2D text;
uniform vec4 color;

void main() {
	vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, tex_coords).r);
	text_color = color * sampled;
}