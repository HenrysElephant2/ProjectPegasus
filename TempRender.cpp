#include "TempRender.h"

RenderSystem::RenderSystem( int width, int height ) {
	char vertFile[] = "Shaders/default.vert";
	char fragFile[] = "Shaders/default.frag";

	defaultProgram = CreateShaderProg(vertFile,fragFile);
	vertexAttrib = glGetAttribLocation( defaultProgram, "Vertex" );

	glClearColor( 0.f, 0.f, 0.f, 1.f );

	//Create VAO
	glGenVertexArrays( 1, &vaoID );
	glBindVertexArray( vaoID );

	//Create VBO
	glGenBuffers( 1, &vboID );
	glBindBuffer( GL_ARRAY_BUFFER, vboID );
	glBufferData( GL_ARRAY_BUFFER, 4 * nVertices * sizeof(GLfloat), vertices, GL_STATIC_DRAW );

	// Initialize matrices
	reshape(width,height);
}

RenderSystem::~RenderSystem() {
	glDeleteProgram( defaultProgram );
}

void RenderSystem::render() {
	glClear( GL_COLOR_BUFFER_BIT );
	glUseProgram(defaultProgram);
	glEnableVertexAttribArray( vertexAttrib );

	glUniformMatrix4fv(glGetUniformLocation(defaultProgram, "Model"), 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(glGetUniformLocation(defaultProgram, "View"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(defaultProgram, "Projection"), 1, GL_FALSE, glm::value_ptr(proj));

	// Draw Quad
	glBindBuffer( GL_ARRAY_BUFFER, vboID );
	glVertexAttribPointer( vertexAttrib, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), NULL );
	glDrawArrays( GL_TRIANGLES, 0, nVertices );

	glDisableVertexAttribArray( vertexAttrib );

	glUseProgram(0);
	debug("RenderSystem");
}

void RenderSystem::update( double dt ) {
	// model = glm::rotate(model, glm::radians(float(dt)*45.0f), glm::vec3(0.0f,-1.0f, 0.0f));
}

void RenderSystem::debug( std::string loc ) { 
	GLenum err;
	while((err = glGetError()) != GL_NO_ERROR)
		std::cout << "Error at " << loc << ": " << err << std::endl;
}

void RenderSystem::setView( float dth, float dph ) {
	th += dth;
	ph += dph;
	createMatrices();
}

void RenderSystem::reshape( int width, int height ) {
	asp = width / (float)height;
	glViewport(0,0,width,height);
	createMatrices();
}

void RenderSystem::createMatrices() {
	proj = glm::perspective(glm::radians(fov), asp , 0.1f, 100.f);
	view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -2.0f));
	model = glm::scale(glm::mat4(1.0f), glm::vec3(0.25f));
	model = glm::rotate(model, glm::radians(ph), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::rotate(model, glm::radians(th), glm::vec3(0.0f, 1.0f, 0.0f));
}
