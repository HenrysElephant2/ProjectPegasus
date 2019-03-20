#ifndef MATERIAL_H
#define MATERIAL_H

#include <glm/glm.hpp>
#include <map>
#include <OpenGL/gl3.h>
#include <iostream>



class TextureLoader{
private:
	std::map<std::string, GLuint> textures; // uses map so it doesn't read a texture multiple time, just returns the GLuint
public:
	TextureLoader();
	GLuint loadTexture(std::string & filename);
};

struct Material{
	glm::vec3 ambient = glm::vec3(0.0,0.0,0.0);
	glm::vec3 diffuse = glm::vec3(0.0,0.0,0.0);
	glm::vec3 specular = glm::vec3(0.0,0.0,0.0);
	glm::vec3 transparentColor = glm::vec3(0.0,0.0,0.0);
	float opacity = 1.0;
	float shininess = 0.0;
	GLuint texture = 0;
	GLuint normals = 0;
	//GLuint specular = 0; // currently not planning on using this, but could be useful for models where glossiness is different on diffferent parts of the model
	//GLuint opacityMap = 0; // not planning on using this, but could be used for cool effects

	// for debugging purposes only
	void print(){
		std::cout << "Ambient: [" << ambient.r << ", " << ambient.g << ", " << ambient.b << "]" << std::endl; 
		std::cout << "Diffuse: [" << diffuse.r << ", " << diffuse.g << ", " << diffuse.b << "]" << std::endl;
		std::cout << "Specular: [" << specular.r << ", " << specular.g << ", " << specular.b << "]" << std::endl;
		std::cout << "Transparent: [" << transparentColor.r << ", " << transparentColor.g << ", " << transparentColor.b << "]" << std::endl;
		std::cout << "Opacity: " << opacity << std::endl;
		std::cout << "Shininess: " << shininess << std::endl;
		std::cout << "Diffuse Texture: " << texture << std::endl;
		std::cout << "Normal Texture: " << normals << std::endl;
	}
};

#endif