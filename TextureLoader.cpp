#include "Material.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

TextureLoader::TextureLoader()
{

}
GLuint TextureLoader::loadTexture(std::string & filename)
{
	if(textures.count(filename) > 0)
		return textures.at(filename);

	
	int width;
	int height;
	int numChannels;
	unsigned char* image = stbi_load(filename.c_str(), &width, &height, &numChannels, 0);
	GLuint tex;
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if(image)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		glGenerateMipmap(GL_TEXTURE_2D);
		stbi_image_free(image);
		textures.insert(std::pair<std::string, GLuint>(filename, tex));
		return tex;
	}
	std::cout << "Failed to load texture: " << filename << std::endl;
	return 0;
}