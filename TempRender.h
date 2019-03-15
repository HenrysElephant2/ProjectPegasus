#ifndef RENDERSYSTEM_H
#define RENDERSYSTEM_H

#include <OpenGL/gl3.h>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp> 
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <iostream>
#include "LoadShader.h"

const GLfloat vertices[] =
{
	// x     y     z     w       r     g     b      nx    ny    nz       u     v
	-1.f,  0.f, -1.f,  1.f,    //1.f,  0.f,  0.f,    0.f, -1.f,  0.f,    0.f,  0.f,
	-1.f,  0.f,  1.f,  1.f,    //1.f,  0.f,  0.f,    0.f, -1.f,  0.f,    0.f,  1.f,
	 1.f,  0.f,  1.f,  1.f,    //1.f,  0.f,  0.f,    0.f, -1.f,  0.f,    1.f,  1.f,
	-1.f,  0.f, -1.f,  1.f,    //1.f,  0.f,  0.f,    0.f, -1.f,  0.f,    0.f,  0.f,
	 1.f,  0.f, -1.f,  1.f,    //1.f,  0.f,  0.f,    0.f, -1.f,  0.f,    1.f,  0.f,
	 1.f,  0.f,  1.f,  1.f//,    1.f,  0.f,  0.f,    0.f, -1.f,  0.f,    1.f,  1.f,
};

const int nVertices = sizeof(vertices)/sizeof(GLfloat)/9;

class RenderSystem {
private:
	float th = 0, ph = 20, asp = 0, fov = 55;

	GLuint defaultProgram;
	GLuint vertexAttrib;
	GLuint vaoID, vboID; // VAO and VBO locations
	
	glm::mat4 proj, view, model;

	void createMatrices();
	void debug( std::string loc );

public:
	RenderSystem( int width, int height );
	~RenderSystem();
	void render();
	void update ( double dt );
	void setView( float th, float ph );
	void reshape( int width, int height );
};

#endif