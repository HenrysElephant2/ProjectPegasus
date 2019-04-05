#ifndef SHADER_MANAGER_H
#define SHADER_MANAGER_H

#include <OpenGL/gl3.h>
#include <vector>
#include <iostream>
#include "Material.h"
#include <glm/mat4x4.hpp> 
#include <glm/gtc/type_ptr.hpp>

//need to #define all the names for shader uniforms
#define MODEL_VARIABLE_NAME "Model"
#define NORMAL_VARIABLE_NAME "NormalMatrix"
#define VIEW_VARIABLE_NAME "View"
#define PROJECTION_VARIABLE_NAME "Projection"

#define AMBIENT_VARIABLE_NAME "ambientMaterialColor"
#define DIFFUSE_VARIABLE_NAME "diffuseMaterialColor"
#define SPECULAR_VARIABLE_NAME "specularMaterialColor"
#define TRANSPARENT_VARIABLE_NAME "transparentMaterialColor"
#define OPACITY_VARIABLE_NAME "materialOpacity"
#define SHININESS_VARIABLE_NAME "materialShininess"
#define COLOR_TEXTURE_VARIABLE_NAME "colorTexture"
#define NORMAL_TEXTURE_VARIABLE_NAME "normalTexture"
#define EMISSIVE_TEXTURE_VARIABLE_NAME "emissiveTexture"

// class shader tries to load all possible uniforms, and if it returns -1 , then don't try to load that uniform when binding material
class Shader
{
private:
	GLuint program;
	GLint modelMatrixLoc;
	GLint normalMatrixLoc;
	GLint viewMatrixLoc;
	GLint projectionMatrixLoc;
	GLint ambientColorLoc;
	GLint diffuseColorLoc;
	GLint specularColorLoc;
	GLint transparentColorLoc;
	GLint opacityLoc;
	GLint shininessLoc;
	GLint colorTextureLoc;
	GLint normalTextureLoc;
	GLint emissiveTextureLoc;
	//GLint specularTextureLoc;
	//GLint opacityTextureLoc;
public:
	Shader(GLuint programID);
	~Shader();
	void loadModelMatrix(glm::mat4 *modelMatrix);
	void loadNormalMatrix(glm::mat3 *normalMatrix);
	void loadViewMatrix(glm::mat4 *viewMatrix);
	void loadProjectionMatrix(glm::mat4 *projectionMatrix);
	void bind();
	void bindMaterial(Material * mat); // load uniforms where the location is not -1, bind textures that are not -1
	GLuint getProgramID();
};

class ShaderManager {
private:
	int current;
	std::vector<Shader *> shaders;
	GLuint loadShaderFromFile(GLenum type, std::string & filename);
public:
	ShaderManager();
	~ShaderManager();
	int createProgram(std::string & vertexShader, std::string & fragShader);
	int createProgram(std::string & vertexShader, std::string & geometryShader, std::string & fragShader);
	void bindShader(int programIndex);
	void bindMaterial(Material * mat);
	void loadModelMatrix(glm::mat4 *modelMatrix);
	void loadNormalMatrix(glm::mat3 *normalMatrix);
	void loadViewMatrix(glm::mat4 *viewMatrix);
	void loadProjectionMatrix(glm::mat4 *projectionMatrix);
	GLuint getProgramID();
	static void loadShaders(ShaderManager* sm);
};

static int hdrProgram;


#endif