#version 330 core

layout (location = 0) out vec4 position;
layout (location = 1) out vec4 normal;
layout (location = 2) out vec4 diffuse;
layout (location = 3) out vec4 emissive;
layout (location = 4) out vec4 occlusion;
layout (location = 5) out vec4 reflectivity;


in vec4 fragPos;
in mat3 toWorld;
in vec2 texCoords;
in float shininess;

uniform sampler2D colorTexture;
uniform sampler2D normalTexture;
uniform sampler2D emissiveTexture;
uniform sampler2D reflectiveTexture;

void main() {
	position = fragPos;
	normal = vec4(normalize((texture(normalTexture, texCoords).rgb * 2.0 - 1.0) * toWorld),1.0);
	diffuse.rgb = texture(colorTexture, texCoords).rgb;
	diffuse.a = shininess;
	emissive = texture(emissiveTexture, texCoords);
	occlusion = vec4(vec3(0.0),1.0);
	reflectivity = texture(reflectiveTexture, texCoords);
}