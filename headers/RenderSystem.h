#ifndef RENDER_SYSTEM_H
#define RENDER_SYSTEM_H

#define GLM_FORCE_SWIZZLE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <SDL2/SDL.h>
#include "ComponentManager.h"
#include "Component.h"
#include "System.h"
#include "MessageManager.h"
#include "ShaderManager.h"
#include "FrameBuffer.h"

#define BLUR_PASSES 8


#define VERTEX_ATTRIB 0
#define RGBA_ATTRIB 1
#define NORM_ATTRIB 2
#define TAN_ATTRIB 3
#define BITAN_ATTRIB 4
#define UV_ATTRIB 5

#define VERTEX_ATTRIB_2D 0
#define UV_ATTRIB_2D 1

static const glm::vec3 xAxis = glm::vec3(1.0,0.0,0.0);
static const glm::vec3 yAxis = glm::vec3(0.0,1.0,0.0);
static const glm::vec3 zAxis = glm::vec3(0.0,0.0,1.0);

static const glm::vec3 lightViews[6] = {
	glm::vec3( 1.0, 0.0, 0.0),
	glm::vec3(-1.0, 0.0, 0.0),
	glm::vec3( 0.0, 1.0, 0.0),
	glm::vec3( 0.0,-1.0, 0.0),
	glm::vec3( 0.0, 0.0, 1.0),
	glm::vec3( 0.0, 0.0,-1.0)
};
static glm::mat4 lightProjection = glm::perspective( 90.0, 1.0, .05, 20.0 );


class RenderSystem:System {
private:
	ComponentManager<Transform> * transforms;
	ComponentManager<Renderable> * renderables;
	ComponentManager<Player> * player;
	ComponentManager<Light> * lights;
	ShaderManager * shaders;
	int cameraID = 0;
	glm::mat4 projection;
	float fov = 55;
	int windowWidth, windowHeight;
	int textureWidth, textureHeight;

	//VAO's
	GLuint BASE_VAO;

	//vbos
	GLuint fullScreenVBO;

	// frame buffers
	// Frame Buffer for shadow mapping
	FrameBuffer shadowMapBuffer;

	// Frame Buffer for testing shadows
	FrameBuffer shadowTestBuffer[2];
	int shadowTestTexture[2];
	int shadowTempTexture[2];
	GLuint shadowTestLightViewLocs[6];
	GLuint shadowTestLightLocLoc;
	GLuint shadowTestLightIndexLoc;
	GLuint shadowTestWindowSizeLoc;

	// frame buffer for storing data for deferred shading
	FrameBuffer deferredShadingData;
	int positionTexture;
	int normalTexture;
	int diffuseTexture;
	int emissiveTexture;

	// render target for all shading calculations. anything added to brightTexture will have bloom applied to it
	FrameBuffer shadingTarget;
	int colorTexture;
	int brightTexture;

	// framebuffers for pingponging bloom
	FrameBuffer bloomBuffer[2];
	int bloomTexture[2];
	GLint horizontalBoolLoc;

	// target to combine color and bloom texture
	FrameBuffer bloomTarget;
	int finalColorTexture;

	// HDR
	GLint exposureLoc;


	//additional info
	GLint cameraPositionUniformLoc;

	void drawAllRenderables( glm::mat4 *viewMat, glm::mat4 *projMat, bool vertex_only = false, bool disableDepth = false );

	// must be used with a shader that is designed to draw a full screen quad. ie the vertex shader shouldn't do any transformations at all to the vertex
	void renderFullScreenQuad();

	void renderShadowMaps();
	void testSingleLight( int componentIndex, int lightIndex, bool bufferIndex, glm::mat4 *viewMat );

	void setUpFrameBuffers();

	void loadLights(std::vector<int> *lightList);

public:
	RenderSystem(MessageManager * m, ShaderManager * sm, ComponentManager<Transform> * transforms_in, ComponentManager<Renderable> * renderables_in, 
				 ComponentManager<Player> * player_in, ComponentManager<Light> * lights_in):System(m)
	{
		shaders = sm;
		transforms = transforms_in;
		renderables = renderables_in;
		player = player_in;
		lights = lights_in;
		glGenVertexArrays( 1, &BASE_VAO );
		glBindVertexArray( BASE_VAO );

		windowHeight = 400;
		windowWidth = 100;

		textureWidth = 2048;
		textureHeight = 1080;

		// create a vbo for displaying vertices 
		float VBO[] = {-1,-1,0,  0,0,  1,-1,0,  1,0,  -1,1,0, 0,1,
				       -1,1, 0,  0,1,  1,-1,0,  1,0,  1,1,0,  1,1};
		glGenBuffers(1, &fullScreenVBO);
	  	glBindBuffer(GL_ARRAY_BUFFER, fullScreenVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 30, &VBO, GL_STATIC_DRAW);

		glViewport(0,0,windowWidth,windowHeight);

		setUpFrameBuffers();

		sm->bindShader(ShaderManager::shadingPass);
		cameraPositionUniformLoc = glGetUniformLocation(sm->getProgramID(), "cameraLoc");
		glUniform1i(glGetUniformLocation(sm->getProgramID(), "positionTexture"), 0);
		glUniform1i(glGetUniformLocation(sm->getProgramID(), "normalTexture"), 1);
		glUniform1i(glGetUniformLocation(sm->getProgramID(), "diffuseTexture"), 2);
		glUniform1i(glGetUniformLocation(sm->getProgramID(), "emissiveTexture"), 3);
		glUniform1i(glGetUniformLocation(sm->getProgramID(), "shadowTexture"), 4);

		sm->bindShader(ShaderManager::HDR);
		exposureLoc = glGetUniformLocation(sm->getProgramID(), "exposure");

		sm->bindShader(ShaderManager::blur);
		horizontalBoolLoc = glGetUniformLocation(sm->getProgramID(), "horizontal");

		sm->bindShader(ShaderManager::applyBloom);
		glUniform1i(glGetUniformLocation(sm->getProgramID(), "baseColor"), 0);
		glUniform1i(glGetUniformLocation(sm->getProgramID(), "bloomColor"), 1);

		sm->bindShader(ShaderManager::testShadows);
		glUniformMatrix4fv( glGetUniformLocation(sm->getProgramID(), "LightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightProjection));
		shadowTestLightIndexLoc = glGetUniformLocation(sm->getProgramID(), "LightIndex");
		shadowTestLightViewLocs[0] = glGetUniformLocation(sm->getProgramID(), "LightViews[0]");
		shadowTestLightViewLocs[1] = glGetUniformLocation(sm->getProgramID(), "LightViews[1]");
		shadowTestLightViewLocs[2] = glGetUniformLocation(sm->getProgramID(), "LightViews[2]");
		shadowTestLightViewLocs[3] = glGetUniformLocation(sm->getProgramID(), "LightViews[3]");
		shadowTestLightViewLocs[4] = glGetUniformLocation(sm->getProgramID(), "LightViews[4]");
		shadowTestLightViewLocs[5] = glGetUniformLocation(sm->getProgramID(), "LightViews[5]");
		shadowTestLightLocLoc = glGetUniformLocation(sm->getProgramID(), "LightLoc");
		shadowTestWindowSizeLoc = glGetUniformLocation(sm->getProgramID(), "WindowSize");
		// Uniform texture locations
		glUniform1i(glGetUniformLocation(sm->getProgramID(), "currentTex"), 1);
		glUniform1i(glGetUniformLocation(sm->getProgramID(), "ShadowMaps[0]"), 2);
		glUniform1i(glGetUniformLocation(sm->getProgramID(), "ShadowMaps[1]"), 3);
		glUniform1i(glGetUniformLocation(sm->getProgramID(), "ShadowMaps[2]"), 4);
		glUniform1i(glGetUniformLocation(sm->getProgramID(), "ShadowMaps[3]"), 5);
		glUniform1i(glGetUniformLocation(sm->getProgramID(), "ShadowMaps[4]"), 6);
		glUniform1i(glGetUniformLocation(sm->getProgramID(), "ShadowMaps[5]"), 7);
		std::cout << "Created RenderSystem" << std::endl;

		sm->bindShader(ShaderManager::tempShadows2);
		glUniform1i(glGetUniformLocation(sm->getProgramID(), "intTex"), 0);
		glUniform1i(glGetUniformLocation(sm->getProgramID(), "tex"), 1);
	}
	RenderSystem():System(NULL){}
	void update();
	void receiveMessage(BasicMessage * message);
	void reshape(int width, int height);
};

#endif