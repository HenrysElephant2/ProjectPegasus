#version 330

uniform vec3 cameraLoc;

uniform sampler2D positionTexture;
uniform sampler2D normalTexture;
uniform sampler2D diffuseTexture;
uniform sampler2D emissiveTexture;

in vec2 texCoords;

out vec4 FragColor;

struct Light {
	vec3 location;
	vec3 diffuse;
	vec3 specular;
	float linearAttenuation;
	float quadraticAttenuation;

};

float tempGamma = 2.2;



void main()
{
	vec3 fragPos = texture(positionTexture, texCoords).rgb;
	vec3 normal = normalize(texture(normalTexture, texCoords).rgb);
	vec3 diffuseColor = texture(diffuseTexture, texCoords).rgb;
	float shininess = texture(diffuseTexture, texCoords).a;

	 vec3 finalColor = diffuseColor * .005; // replace diffuseColor with ambient color
	//vec3 finalColor = vec3(0.0,0.0,0.0);

	vec3 lightPos = vec3(0.0,1.0,0.0);
	float linear = 1.0;
	float quadratic = 0.0;//0001;

	vec3 viewDirection = normalize(cameraLoc - fragPos);



	//for all lights
	{
		vec3 lightDirection = normalize(lightPos - fragPos);
		float diffuseAmount = max(dot(normal, lightDirection),0.0);
		
		vec3 diffuse = diffuseColor * diffuseAmount * vec3(0.3,0.3,0.3); // * lights[i].diffuseColor

		vec3 R = reflect(-lightDirection,normal);
		vec3 halfwaydir = normalize(lightDirection + viewDirection);

		 //float specularAmount = max(dot(normal,halfwaydir),0.0);
		float specularAmount = max(dot(R,viewDirection),0.0);
		// if(diffuseAmount == 0)
		// 	specularAmount = 0;

		vec3 specular = vec3(0.0);
		if(diffuseAmount > 0.0)
			specular = pow(specularAmount,shininess) * vec3(0.5,0.5,0.5);//* lights[i].specularColor;


		float d = length(lightPos - fragPos);
		float attenuation = 1.0 / (1.0 + linear * d + quadratic * d * d);

		finalColor += diffuse * attenuation + specular * attenuation;

	}

	finalColor += texture(emissiveTexture, texCoords).xyz;
	FragColor = vec4(finalColor, 1.0);
	
}
