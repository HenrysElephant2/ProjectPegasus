#version 330 core

layout (location = 0) out uint FragColor;

in vec2 texCoords;

uniform mat4 LightView;
uniform mat4 LightProjection;

uniform vec3 LightLoc;
uniform int LightIndex;

uniform usampler2D currentTex;
uniform sampler2D positionTexture;
uniform sampler2D normalTexture;

uniform sampler2D ShadowMap;

void main() {
	vec3 loc = texture(positionTexture, texCoords).rgb;
	vec3 norm = normalize(texture(normalTexture, texCoords).rgb);

	vec3 lightDir = -LightLoc;
	float lit = 1.0;

	if( dot(norm, lightDir) < 0.0 ) {
		vec4 lightSpaceLoc = LightProjection * LightView * vec4(loc,1.0);
		vec3 projloc = (lightSpaceLoc.xyz / lightSpaceLoc.w) * 0.5 + 0.5;
		if( projloc.x >= 0 && projloc.x <= 1.0 && projloc.y >= 0 && projloc.y <= 1.0 ) {
			float closestDepth = texture(ShadowMap, projloc.xy).r; 

			// get depth of current fragment from light's perspective
			float currentDepth = projloc.z;
			// check whether current frag pos is in shadow
			float bias = 0.000013;
			//float bias = max(0.00001 * (1.0 - dot(norm, lightDir)), 0.000001); 
			lit = currentDepth - bias < closestDepth ? 1.0 : 0.0;
		}
	}

	uint current = texture(currentTex, texCoords).r;

	// Set corresponding bit of integer texture to reflect shadow value
	FragColor = current | (uint(lit) << (LightIndex));
}
