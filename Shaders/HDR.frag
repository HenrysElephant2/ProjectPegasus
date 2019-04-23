#version 330

in vec2 texCoords;

out vec4 FragColor;

uniform sampler2D colorTexture;
uniform float exposure;

const float gamma = 2.2;
void main() {
	vec3 color = texture(colorTexture, texCoords).rgb;
	vec3 result = vec3(1.0) - exp(-color * exposure);
	result = pow(result, vec3(1.0/gamma));
	FragColor = vec4(result,1.0);
	// FragColor = texture(colorTexture, texCoords);
}