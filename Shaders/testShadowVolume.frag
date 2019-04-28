#version 330 core

layout (location = 0) out uint FragColor;
layout (location = 1) out vec4 TempColor;

in vec4 loc;
in vec4 norm;
in vec4 lightSpaceLoc[6];

uniform vec3 LightLoc;
uniform int LightIndex;
uniform usampler2D currentTex;
uniform sampler2D ShadowMap0;
uniform sampler2D ShadowMap1;
uniform sampler2D ShadowMap2;
uniform sampler2D ShadowMap3;
uniform sampler2D ShadowMap4;
uniform sampler2D ShadowMap5;
uniform vec2 WindowSize;

const float near_plane = 0.05;
const float far_plane  = 20.0;

float LinearizeDepth(float depth)
{
	float z = depth * 2.0 - 1.0; // Back to NDC 
	return (2.0 * near_plane * far_plane) / (far_plane + near_plane - z * (far_plane - near_plane));
}

void main() {
	vec3 lightDir = loc.xyz - LightLoc;
	float lit = 0.0;

	if( dot(norm.xyz, lightDir) < 0.0 ) {
		vec3 projloc;
		float closestDepth;

		if(abs(lightDir.x) > max(abs(lightDir.y),abs(lightDir.z))) {
			if( lightDir.x > 0 ) {
				projloc = (lightSpaceLoc[0].xyz / lightSpaceLoc[0].w) * 0.5 + 0.5;
				closestDepth = texture(ShadowMap0, projloc.xy).r; 
			}
			else {
				projloc = (lightSpaceLoc[1].xyz / lightSpaceLoc[1].w) * 0.5 + 0.5;
				closestDepth = texture(ShadowMap1, projloc.xy).r; 
			}
		}
		else if(abs(lightDir.y) > abs(lightDir.z)) {
			if( lightDir.y > 0 ) {
				projloc = (lightSpaceLoc[2].xyz / lightSpaceLoc[2].w) * 0.5 + 0.5;
				closestDepth = texture(ShadowMap2, projloc.xy).r; 
			}
			else {
				projloc = (lightSpaceLoc[3].xyz / lightSpaceLoc[3].w) * 0.5 + 0.5;
				closestDepth = texture(ShadowMap3, projloc.xy).r; 
			}
		}
		else{
			if( lightDir.z > 0 ) {
				projloc = (lightSpaceLoc[4].xyz / lightSpaceLoc[4].w) * 0.5 + 0.5;
				closestDepth = texture(ShadowMap4, projloc.xy).r; 
			}
			else {
				projloc = (lightSpaceLoc[5].xyz / lightSpaceLoc[5].w) * 0.5 + 0.5;
				closestDepth = texture(ShadowMap5, projloc.xy).r; 
			}
		}

		// get depth of current fragment from light's perspective
		float currentDepth = projloc.z;
		// check whether current frag pos is in shadow
		float bias = 0.0005;
		lit = currentDepth - bias < closestDepth ? 1.0 : 0.0;
	}

	vec3 testLoc = gl_FragCoord.xyz; // / gl_FragCoord.w;
	vec2 texCoords = vec2(testLoc.x/WindowSize.x, testLoc.y/WindowSize.y);
	uint current = texture(currentTex, texCoords.xy).r;

	// Set corresponding bit of integer texture to reflect shadow value
	FragColor = current | (uint(lit) << (LightIndex));
}
