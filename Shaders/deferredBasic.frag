#version 330

layout (location = 0) out vec4 position;
layout (location = 1) out vec4 normal;
layout (location = 2) out vec4 diffuse;
layout (location = 3) out vec4 emissive;


in vec4 fragPos;
in vec3 fragNormal;
in mat3 toWorld;
in vec2 texCoords;
in float shininess;

uniform sampler2D colorTexture;
uniform sampler2D normalTexture;// just for testing, this shader shouldn't do normal mapping

void main() {
	position = fragPos;
	//normal = vec4(normalize((texture(normalTexture, texCoords).rgb * 2.0 - 1.0) * toWorld),1.0);
	normal = vec4(fragNormal,1.0);
	diffuse.rgb = texture(colorTexture, texCoords).rgb;
	diffuse.a = shininess;
}