#version 330

// Gaussian blur shader for applying bloom

uniform bool horizontal;
uniform sampler2D colorTexture;

in vec2 texCoords;

out vec4 FragColor;

float weight[5] = float[5](0.2270270270, 0.1945945946, 0.1216216216, 0.0540540541, 0.0162162162);
vec2 texelSize = 1.0 / textureSize(colorTexture, 0);

vec4 sample(float dx,float dy)
{
   return texture(colorTexture,texCoords+texelSize * vec2(dx,dy));
}

void main()
{
   	vec2 offset;
   	if(horizontal)
   		offset = vec2(texelSize.x, 0.0);
   	else offset = vec2(0.0, texelSize.y);

   	vec3 result = texture(colorTexture, texCoords).rgb * weight[0];
   	for(int i = 1; i < 5; i++)
   	{
   		result += texture(colorTexture, texCoords + offset * i).rgb * weight[i];
   		result += texture(colorTexture, texCoords - offset * i).rgb * weight[i];
   	}

   FragColor = vec4(result,1.0);
}
