#ifndef RENDER_SYSTEM_H
#define RENDER_SYSTEM_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <SDL2/SDL.h>
#include <cstdlib>
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
#define BONE_WEIGHTS_ATTRIB 6
#define BONE_IDS_ATTRIB 7


#define VERTEX_ATTRIB_2D 0
#define UV_ATTRIB_2D 1

static const glm::vec3 xAxis = glm::vec3(1.0,0.0,0.0);
static const glm::vec3 yAxis = glm::vec3(0.0,1.0,0.0);
static const glm::vec3 zAxis = glm::vec3(0.0,0.0,1.0);

static glm::mat4 lightProjection = glm::perspective( 90.0, 1.0, .05, 100.0 );
static glm::mat4 lightOrthoProjection = glm::ortho( -40.0, 40.0, -40.0, 40.0, -100.0, 100.0 );


class RenderSystem:System {
private:
	ComponentManager<Transform> * transforms;
	ComponentManager<Renderable> * renderables;
	ComponentManager<SkinnedRenderable> * skinnedRenderables;
	ComponentManager<Player> * player;
	ComponentManager<Light> * lights;
	ComponentManager<ParticleSystem> * particleSystems;
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

	GLuint directionalShadowTestLightViewLoc;
	GLuint directionalShadowTestLightLocLoc;
	GLuint directionalShadowTestLightIndexLoc;

	// frame buffer for storing data for deferred shading
	FrameBuffer deferredShadingData;
	int positionTexture;
	int normalTexture;
	int diffuseTexture;
	int emissiveTexture;
	int occlusionTexture;

	// render target for all shading calculations. anything added to brightTexture will have bloom applied to it
	FrameBuffer shadingTarget;
	int colorTexture;
	int brightTexture;

	// Framebuffer and textures for particle calculation
	GLuint pTimeLoc;
	float ptime = 0;

	// framebuffers for pingponging bloom
	FrameBuffer bloomBuffer[2];
	int bloomTexture[2];
	GLint horizontalBoolLoc;

	// target to combine color and bloom texture
	FrameBuffer bloomTarget;
	int finalColorTexture;

	// uniform variables for volumetric light scattering
	int sunID = -1;
	GLuint sunScreenCoordLoc;
	GLuint densityLoc;
	GLuint weightLoc;
	GLuint decayLoc;

	// HDR
	GLint exposureLoc;


	//additional info
	GLint cameraPositionUniformLoc;

	// Draw everything
	void drawAllRenderables( glm::mat4 *viewMat, glm::mat4 *projMat, bool vertex_only = false );
	void drawSkinnedRenderables( glm::mat4 *viewMat, glm::mat4 *projMat, bool vertex_only = false );
	void renderParticleSystems( glm::mat4 *viewMat, glm::mat4 *projMat );

	// must be used with a shader that is designed to draw a full screen quad. ie the vertex shader shouldn't do any transformations at all to the vertex
	void renderFullScreenQuad();

	// Create per-light shadow maps
	void renderShadowMaps( glm::vec3 playerLoc );

	// Perform shadow tests for a single light
	void testSingleLight( int componentIndex, int lightIndex, bool bufferIndex );
	void testSingleDirectionalLight( int componentIndex, int lightIndex, bool bufferIndex, glm::vec3 playerLoc );

	void setUpFrameBuffers();

	void loadLights(std::vector<int> *lightList);

	static glm::vec3 lightViews[6];

public:
	RenderSystem(MessageManager * m, ComponentManager<Transform> * transforms_in, ComponentManager<Renderable> * renderables_in, 
				 ComponentManager<SkinnedRenderable> * skinnedRenderables_in, ComponentManager<Player> * player_in, ComponentManager<Light> * lights_in, ComponentManager<ParticleSystem> * ps_in );
	RenderSystem():System(NULL){}
	void update();
	void receiveMessage(BasicMessage * message);
	void reshape(int width, int height);
};

#endif