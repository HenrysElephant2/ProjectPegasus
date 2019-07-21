#version 330

uniform vec3 cameraLoc;

uniform sampler2D positionTexture;
uniform sampler2D normalTexture;
uniform sampler2D diffuseTexture;
uniform sampler2D emissiveTexture;
uniform usampler2D shadowTexture;

in vec2 texCoords;

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 brightColor;

struct Light {
	vec3 location;
	vec3 diffuse;
	vec3 specular;
	float linearAttenuation;
	float quadraticAttenuation;
	bool directional;
	vec3 direction;
};

const int MAX_LIGHTS = 32;
uniform Light lights[MAX_LIGHTS];
uniform int numLights;

float tempGamma = 2.2;

const vec3 temp = vec3(0.0,-1.0,0.0);

void main()
{
	vec3 fragPos = texture(positionTexture, texCoords).rgb;
	vec3 normal = normalize(texture(normalTexture, texCoords).rgb);
	vec3 diffuseColor = texture(diffuseTexture, texCoords).rgb;
	float shininess = texture(diffuseTexture, texCoords).a;
	uint shadow = texture(shadowTexture, texCoords).r;

	vec3 finalColor = diffuseColor * 0.005; // replace diffuseColor with ambient color
	//vec3 finalColor = vec3(0.0,0.0,0.0);

	// vec3 lightPos = vec3(0.0,1.0,0.0);
	// float linear = 1.0;
	// float quadratic = 0.0;//0001;

	vec3 viewDirection = normalize(cameraLoc - fragPos);

	brightColor = vec4(0.0);

	for(int i = 0; i < numLights && i < MAX_LIGHTS; i++) //numLights
	{
		float shadowVal = float(shadow & (1u << i));
		

		vec3 lightDirection = lights[i].directional ? -normalize(lights[i].direction) : normalize(lights[i].location - fragPos);
		float diffuseAmount = max(dot(normal, lightDirection),0.0);
		
		vec3 diffuse = diffuseColor * diffuseAmount * lights[i].diffuse;

		vec3 R = reflect(-lightDirection,normal);
		vec3 halfwaydir = normalize(lightDirection + viewDirection);

		//float specularAmount = max(dot(normal,halfwaydir),0.0);
		float specularAmount = max(dot(R,viewDirection),0.0);
		

		vec3 specular = vec3(0.0);
		if(diffuseAmount > 0.0)
			specular = pow(specularAmount,shininess) * lights[i].specular;


		float d = length(lights[i].location - fragPos);
		float attenuation = lights[i].directional ? 1.0 : 1.0 / (1.0 + lights[i].linearAttenuation * d + lights[i].quadraticAttenuation * d * d);

		finalColor += shadowVal * (diffuse * attenuation + specular * attenuation);

		brightColor += vec4(shadowVal * (specular * attenuation),0.0);

	}

	finalColor += texture(emissiveTexture, texCoords).xyz;
	FragColor = vec4(finalColor, 1.0);
	brightColor += texture(emissiveTexture, texCoords) * texture(emissiveTexture,texCoords).a;

	// FragColor = vec4((normal + vec3(1.0))/2.0,1.0);
	// brightColor = vec4(0.0);
}
