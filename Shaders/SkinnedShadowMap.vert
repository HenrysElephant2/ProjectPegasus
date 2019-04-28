#version 330

layout(location = 0) in vec4 Vertex;
layout(location = 2) in vec4 Normal;
layout(location = 6) in vec4 Weights;
layout(location = 7) in ivec4 BoneIDs;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

const int MAX_BONES = 100;
uniform mat4 BoneTransforms[MAX_BONES];

void main() {
	mat4 transform = BoneTransforms[BoneIDs.x] * Weights.x + BoneTransforms[BoneIDs.y] * Weights.y + BoneTransforms[BoneIDs.z] * Weights.z + BoneTransforms[BoneIDs.w] * Weights.w;
	gl_Position = Projection * View * Model * (transform * Vertex);
}