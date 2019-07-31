#version 330

layout (location = 0) out vec4 diffuse;


in vec4 fragNormal;
in vec2 texCoords;

uniform sampler2D colorTexture;
uniform sampler2D emissiveTexture;

void main() {
	vec4 emission = texture(emissiveTexture,texCoords);
	diffuse = texture(colorTexture,texCoords);
	diffuse.w = (emission.x + emission.y + emission.z) / 3.0;
}