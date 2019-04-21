#version 330

layout(location = 0) in vec4 Vertex;
layout(location = 1) in vec4 Color;
layout(location = 2) in vec3 Normal;
layout(location = 3) in vec3 Tangent;
layout(location = 4) in vec3 Bitangent;
layout(location = 5) in vec2 Texture;
layout(location = 6) in ivec4 BoneIDs;
layout(location = 7) in vec4 Weights;

uniform mat4 Model;
uniform mat3 NormalMatrix;
uniform mat4 View;
uniform mat4 Projection;

const int MAX_BONES = 100;
uniform mat4 BoneTransforms[MAX_BONES];

uniform float materialShininess;

out vec4 fragPos;
out vec3 fragNormal;
out vec2 texCoords;
out float shininess;

void main() {
	mat4 transform = BoneTransforms[BoneIDs.x] * Weights.x + BoneTransforms[BoneIDs.y] * Weights.y +
					 BoneTransforms[BoneIDs.z] * Weights.z + BoneTransforms[BoneIDs.w] * Weights.w;
	vec4 VertexFinal = transform * Vertex;
	vec4 NormalFinal = transform * Normal;
	vec3 worldspaceNormal = normalize(NormalFinal * NormalMatrix);

	vec4 worldVertexPosition = Model * VertexFinal;
	fragPos = worldVertexPosition / worldVertexPosition.w;
	fragNormal = worldspaceNormal;
	texCoords = Texture;
	shininess = materialShininess;
	gl_Position = Projection * View * worldVertexPosition;
}