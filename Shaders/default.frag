#version 330

in vec4 col;
in vec3 norm;
in vec3 tan;
in vec3 bitan;
in vec2 tex;

out vec4 FragColor;

uniform sampler2D texSampler;

void main() {
	FragColor = col * texture(texSampler, tex);
}