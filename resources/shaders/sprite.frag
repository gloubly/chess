#version 460 core

out vec4 frag_color;

in vec2 tex_coords;

uniform sampler2D texture0;
uniform vec4 color;

void main() {	
	vec4 tex_color = texture(texture0, tex_coords);
	if(tex_color.r < 0.1) {
		discard;
	}
    frag_color = vec4(color.xyz * tex_color.r, color.a);
}