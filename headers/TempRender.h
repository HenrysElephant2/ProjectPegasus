#ifndef RENDERSYSTEM_H
#define RENDERSYSTEM_H


#define GL_GLEXT_PROTOTYPES 1
#include <SDL2/SDL_opengl.h>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp> 
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <iostream>
#include "LoadShader.h"
#include "LevelLoader.h"
#include "ShaderManager.h"



#define N_VERTEX_VALUES 19
const GLfloat vertices[] =
{
	// x     y     z     w       r     g     b     a      nx    ny    nz      tx    ty    tz     btx   bty   btz       u     v
	-1.f,  1.f, -1.f,  1.f,    1.f,  0.f,  0.f,  1.f,    0.f,  1.f,  0.f,    0.f,  0.f,  0.f,    0.f,  0.f,  0.f,    0.f,  0.f,
	-1.f,  1.f,  1.f,  1.f,    1.f,  0.f,  0.f,  1.f,    0.f,  1.f,  0.f,    0.f,  0.f,  0.f,    0.f,  0.f,  0.f,    0.f,  1.f,
	 1.f,  1.f,  1.f,  1.f,    1.f,  0.f,  0.f,  1.f,    0.f,  1.f,  0.f,    0.f,  0.f,  0.f,    0.f,  0.f,  0.f,    1.f,  1.f,
	-1.f,  1.f, -1.f,  1.f,    1.f,  0.f,  0.f,  1.f,    0.f,  1.f,  0.f,    0.f,  0.f,  0.f,    0.f,  0.f,  0.f,    0.f,  0.f,
	 1.f,  1.f, -1.f,  1.f,    1.f,  0.f,  0.f,  1.f,    0.f,  1.f,  0.f,    0.f,  0.f,  0.f,    0.f,  0.f,  0.f,    1.f,  0.f,
	 1.f,  1.f,  1.f,  1.f,    1.f,  0.f,  0.f,  1.f,    0.f,  1.f,  0.f,    0.f,  0.f,  0.f,    0.f,  0.f,  0.f,    1.f,  1.f,

	-1.f, -1.f, -1.f,  1.f,    1.f,  1.f,  0.f,  1.f,    0.f, -1.f,  0.f,    0.f,  0.f,  0.f,    0.f,  0.f,  0.f,    0.f,  0.f,
	-1.f, -1.f,  1.f,  1.f,    1.f,  1.f,  0.f,  1.f,    0.f, -1.f,  0.f,    0.f,  0.f,  0.f,    0.f,  0.f,  0.f,    0.f,  1.f,
	 1.f, -1.f,  1.f,  1.f,    1.f,  1.f,  0.f,  1.f,    0.f, -1.f,  0.f,    0.f,  0.f,  0.f,    0.f,  0.f,  0.f,    1.f,  1.f,
	-1.f, -1.f, -1.f,  1.f,    1.f,  1.f,  0.f,  1.f,    0.f, -1.f,  0.f,    0.f,  0.f,  0.f,    0.f,  0.f,  0.f,    0.f,  0.f,
	 1.f, -1.f, -1.f,  1.f,    1.f,  1.f,  0.f,  1.f,    0.f, -1.f,  0.f,    0.f,  0.f,  0.f,    0.f,  0.f,  0.f,    1.f,  0.f,
	 1.f, -1.f,  1.f,  1.f,    1.f,  1.f,  0.f,  1.f,    0.f, -1.f,  0.f,    0.f,  0.f,  0.f,    0.f,  0.f,  0.f,    1.f,  1.f,

	 1.f, -1.f, -1.f,  1.f,    0.f,  1.f,  0.f,  1.f,    1.f,  0.f,  0.f,    0.f,  0.f,  0.f,    0.f,  0.f,  0.f,    0.f,  0.f,
	 1.f, -1.f,  1.f,  1.f,    0.f,  1.f,  0.f,  1.f,    1.f,  0.f,  0.f,    0.f,  0.f,  0.f,    0.f,  0.f,  0.f,    0.f,  1.f,
	 1.f,  1.f,  1.f,  1.f,    0.f,  1.f,  0.f,  1.f,    1.f,  0.f,  0.f,    0.f,  0.f,  0.f,    0.f,  0.f,  0.f,    1.f,  1.f,
	 1.f, -1.f, -1.f,  1.f,    0.f,  1.f,  0.f,  1.f,    1.f,  0.f,  0.f,    0.f,  0.f,  0.f,    0.f,  0.f,  0.f,    0.f,  0.f,
	 1.f,  1.f, -1.f,  1.f,    0.f,  1.f,  0.f,  1.f,    1.f,  0.f,  0.f,    0.f,  0.f,  0.f,    0.f,  0.f,  0.f,    1.f,  0.f,
	 1.f,  1.f,  1.f,  1.f,    0.f,  1.f,  0.f,  1.f,    1.f,  0.f,  0.f,    0.f,  0.f,  0.f,    0.f,  0.f,  0.f,    1.f,  1.f,

	-1.f, -1.f, -1.f,  1.f,    0.f,  1.f,  1.f,  1.f,   -1.f,  0.f,  0.f,    0.f,  0.f,  0.f,    0.f,  0.f,  0.f,    0.f,  0.f,
	-1.f, -1.f,  1.f,  1.f,    0.f,  1.f,  1.f,  1.f,   -1.f,  0.f,  0.f,    0.f,  0.f,  0.f,    0.f,  0.f,  0.f,    0.f,  1.f,
	-1.f,  1.f,  1.f,  1.f,    0.f,  1.f,  1.f,  1.f,   -1.f,  0.f,  0.f,    0.f,  0.f,  0.f,    0.f,  0.f,  0.f,    1.f,  1.f,
	-1.f, -1.f, -1.f,  1.f,    0.f,  1.f,  1.f,  1.f,   -1.f,  0.f,  0.f,    0.f,  0.f,  0.f,    0.f,  0.f,  0.f,    0.f,  0.f,
	-1.f,  1.f, -1.f,  1.f,    0.f,  1.f,  1.f,  1.f,   -1.f,  0.f,  0.f,    0.f,  0.f,  0.f,    0.f,  0.f,  0.f,    1.f,  0.f,
	-1.f,  1.f,  1.f,  1.f,    0.f,  1.f,  1.f,  1.f,   -1.f,  0.f,  0.f,    0.f,  0.f,  0.f,    0.f,  0.f,  0.f,    1.f,  1.f,

	-1.f, -1.f,  1.f,  1.f,    0.f,  0.f,  1.f,  1.f,    0.f,  0.f,  1.f,    0.f,  0.f,  0.f,    0.f,  0.f,  0.f,    0.f,  0.f,
	-1.f,  1.f,  1.f,  1.f,    0.f,  0.f,  1.f,  1.f,    0.f,  0.f,  1.f,    0.f,  0.f,  0.f,    0.f,  0.f,  0.f,    0.f,  1.f,
	 1.f,  1.f,  1.f,  1.f,    0.f,  0.f,  1.f,  1.f,    0.f,  0.f,  1.f,    0.f,  0.f,  0.f,    0.f,  0.f,  0.f,    1.f,  1.f,
	-1.f, -1.f,  1.f,  1.f,    0.f,  0.f,  1.f,  1.f,    0.f,  0.f,  1.f,    0.f,  0.f,  0.f,    0.f,  0.f,  0.f,    0.f,  0.f,
	 1.f, -1.f,  1.f,  1.f,    0.f,  0.f,  1.f,  1.f,    0.f,  0.f,  1.f,    0.f,  0.f,  0.f,    0.f,  0.f,  0.f,    1.f,  0.f,
	 1.f,  1.f,  1.f,  1.f,    0.f,  0.f,  1.f,  1.f,    0.f,  0.f,  1.f,    0.f,  0.f,  0.f,    0.f,  0.f,  0.f,    1.f,  1.f,

	-1.f, -1.f, -1.f,  1.f,    1.f,  0.f,  1.f,  1.f,    0.f,  0.f, -1.f,    0.f,  0.f,  0.f,    0.f,  0.f,  0.f,    0.f,  0.f,
	-1.f,  1.f, -1.f,  1.f,    1.f,  0.f,  1.f,  1.f,    0.f,  0.f, -1.f,    0.f,  0.f,  0.f,    0.f,  0.f,  0.f,    0.f,  1.f,
	 1.f,  1.f, -1.f,  1.f,    1.f,  0.f,  1.f,  1.f,    0.f,  0.f, -1.f,    0.f,  0.f,  0.f,    0.f,  0.f,  0.f,    1.f,  1.f,
	-1.f, -1.f, -1.f,  1.f,    1.f,  0.f,  1.f,  1.f,    0.f,  0.f, -1.f,    0.f,  0.f,  0.f,    0.f,  0.f,  0.f,    0.f,  0.f,
	 1.f, -1.f, -1.f,  1.f,    1.f,  0.f,  1.f,  1.f,    0.f,  0.f, -1.f,    0.f,  0.f,  0.f,    0.f,  0.f,  0.f,    1.f,  0.f,
	 1.f,  1.f, -1.f,  1.f,    1.f,  0.f,  1.f,  1.f,    0.f,  0.f, -1.f,    0.f,  0.f,  0.f,    0.f,  0.f,  0.f,    1.f,  1.f,
};
const GLint indexBufferData[] = {
	0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35
};

const int nVertices = sizeof(vertices)/sizeof(GLfloat)/N_VERTEX_VALUES;

// Locations of vertex attributes in shader
const GLint vertexAttrib = 0;
const GLint rgbaAttrib   = 1;
const GLint normAttrib   = 2;
const GLint tanAttrib    = 3;
const GLint bitanAttrib  = 4;
const GLint texAttrib    = 5;

class RenderSystem {
private:
	float th = 0, ph = 20, asp = 0, fov = 55;

	int defaultProgram;
	GLuint vaoID, vboID, iboID; // VAO and VBO locations
	Scene sc;
	ShaderManager sm;
	
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