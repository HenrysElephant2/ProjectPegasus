#version 330

in vec2 texCoords;

out vec4 FragColor;

uniform sampler2D colorTexture;


void main() {
	FragColor = texture(colorTexture, texCoords);
}