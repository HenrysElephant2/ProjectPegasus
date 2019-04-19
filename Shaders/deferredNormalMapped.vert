#version 330 core

layout(location = 0) in vec4 Vertex;
layout(location = 1) in vec4 Color;
layout(location = 2) in vec3 Normal;
layout(location = 3) in vec3 Tangent;
layout(location = 4) in vec3 Bitangent;
layout(location = 5) in vec2 Texture;

uniform mat4 Model;
uniform mat3 NormalMatrix;
uniform mat4 View;
uniform mat4 Projection;

uniform float materialShininess;

out vec4 fragPos;
out mat3 toWorld;
out vec2 texCoords;
out float shininess;

void main() {
	vec3 worldspaceTangent = normalize(Tangent * NormalMatrix);
	vec3 worldspaceBitangent = normalize(Bitangent * NormalMatrix);
	vec3 worldspaceNormal = normalize(Normal * NormalMatrix);

	vec4 worldVertexPosition = Model * Vertex;
	fragPos = worldVertexPosition / worldVertexPosition.w;
	texCoords = Texture;
	shininess = materialShininess;

	toWorld = mat3(worldspaceTangent.x, worldspaceBitangent.x, worldspaceNormal.x,
				   worldspaceTangent.y, worldspaceBitangent.y, worldspaceNormal.y,
				   worldspaceTangent.z, worldspaceBitangent.z, worldspaceNormal.z);
	gl_Position = Projection * View * worldVertexPosition;
}