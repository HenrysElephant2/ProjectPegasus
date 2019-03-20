#include "TempRender.h"

RenderSystem::RenderSystem( int width, int height ) {
	// Create Shader
	char vertFile[] = "Shaders/default.vert";
	char fragFile[] = "Shaders/default.frag";
	defaultProgram = CreateShaderProg(vertFile,fragFile);

	// Get attribute locations -- recommended to just use constant attribute locations (defined in header)
	// vertexAttrib   = glGetAttribLocation( defaultProgram, "Vertex" );
	// rgbaAttrib     = glGetAttribLocation( defaultProgram, "Color" );
	// normAttrib     = glGetAttribLocation( defaultProgram, "Normal" );
	// tanAttrib      = glGetAttribLocation( defaultProgram, "Tangent" );
	// bitanAttrib    = glGetAttribLocation( defaultProgram, "Bitangent" );
	// texAttrib      = glGetAttribLocation( defaultProgram, "Texture" );

	glClearColor( 0.f, 0.f, 0.f, 1.f );

	// Create VAO
	glGenVertexArrays( 1, &vaoID );
	glBindVertexArray( vaoID );

	// Create VBO
	glGenBuffers( 1, &vboID );
	glBindBuffer( GL_ARRAY_BUFFER, vboID );
	glBufferData( GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW ); // 4 * nVertices * sizeof(GLfloat)

	// Create IBO
	glGenBuffers( 1, &iboID );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, iboID );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof(indexBufferData), indexBufferData, GL_STATIC_DRAW );

	// Initialize matrices
	reshape(width,height);
	sc = Scene();
	std::string filename = "testwarehouse.fbx";
	sc.openFile(filename);
}

RenderSystem::~RenderSystem() {
	glDeleteProgram( defaultProgram );
}

void RenderSystem::render() {
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glEnable(GL_DEPTH_TEST);

	glUseProgram(defaultProgram);
	glEnableVertexAttribArray( vertexAttrib );
	glEnableVertexAttribArray( rgbaAttrib );
	glEnableVertexAttribArray( normAttrib );
	glEnableVertexAttribArray( tanAttrib );
	glEnableVertexAttribArray( bitanAttrib );
	glEnableVertexAttribArray( texAttrib );

	glUniformMatrix4fv(glGetUniformLocation(defaultProgram, "Model"), 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(glGetUniformLocation(defaultProgram, "View"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(defaultProgram, "Projection"), 1, GL_FALSE, glm::value_ptr(proj));

	// Draw Quad
	glBindBuffer( GL_ARRAY_BUFFER, vboID );
	glVertexAttribPointer( vertexAttrib, 4, GL_FLOAT, GL_FALSE, N_VERTEX_VALUES * sizeof(GLfloat), (GLvoid*)(0) );
	glVertexAttribPointer( rgbaAttrib,   4, GL_FLOAT, GL_FALSE, N_VERTEX_VALUES * sizeof(GLfloat), (GLvoid*)(4 *sizeof(GLfloat)) );
	glVertexAttribPointer( normAttrib,   3, GL_FLOAT, GL_FALSE, N_VERTEX_VALUES * sizeof(GLfloat), (GLvoid*)(8 *sizeof(GLfloat)) );
	glVertexAttribPointer( tanAttrib,    3, GL_FLOAT, GL_FALSE, N_VERTEX_VALUES * sizeof(GLfloat), (GLvoid*)(11*sizeof(GLfloat)) );
	glVertexAttribPointer( bitanAttrib,  3, GL_FLOAT, GL_FALSE, N_VERTEX_VALUES * sizeof(GLfloat), (GLvoid*)(14*sizeof(GLfloat)) );
	glVertexAttribPointer( texAttrib,    2, GL_FLOAT, GL_FALSE, N_VERTEX_VALUES * sizeof(GLfloat), (GLvoid*)(17*sizeof(GLfloat)) );
	glDrawElements( GL_TRIANGLES, nVertices, GL_UNSIGNED_INT, 0 );

	sc.render();

	glDisableVertexAttribArray( vertexAttrib );
	glDisableVertexAttribArray( rgbaAttrib );
	glDisableVertexAttribArray( normAttrib );
	glDisableVertexAttribArray( tanAttrib );
	glDisableVertexAttribArray( bitanAttrib );
	glDisableVertexAttribArray( texAttrib );

	glUseProgram(0);
	glDisable(GL_DEPTH_TEST);
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
