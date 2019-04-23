#version 330

in vec2 texCoords;

out vec4 FragColor;

uniform usampler2D intTex;
uniform sampler2D tex;

void main()
{
	uint sampled = (texture(intTex, texCoords).r & (1u << 3));
	FragColor = vec4(vec3(float(sampled)), 1.0);

	// FragColor = vec4(texture(tex, texCoords).r);
}