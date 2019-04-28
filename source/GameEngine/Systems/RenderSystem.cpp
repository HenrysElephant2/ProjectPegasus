#include "RenderSystem.h"

glm::vec3 RenderSystem::lightViews[6] = {
	glm::vec3( 1.0, 0.0, 0.0),
	glm::vec3(-1.0, 0.0, 0.0),
	glm::vec3( 0.0, 1.0, 0.0),
	glm::vec3( 0.0,-1.0, 0.0),
	glm::vec3( 0.0, 0.0, 1.0),
	glm::vec3( 0.0, 0.0,-1.0)
};

RenderSystem::RenderSystem(MessageManager * m, ShaderManager * sm, ComponentManager<Transform> * transforms_in, ComponentManager<Renderable> * renderables_in, 
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
		glUniform1i(glGetUniformLocation(sm->getProgramID(), "ShadowMap0"), 2);
		glUniform1i(glGetUniformLocation(sm->getProgramID(), "ShadowMap1"), 3);
		glUniform1i(glGetUniformLocation(sm->getProgramID(), "ShadowMap2"), 4);
		glUniform1i(glGetUniformLocation(sm->getProgramID(), "ShadowMap3"), 5);
		glUniform1i(glGetUniformLocation(sm->getProgramID(), "ShadowMap4"), 6);
		glUniform1i(glGetUniformLocation(sm->getProgramID(), "ShadowMap5"), 7);
		std::cout << "Created RenderSystem" << std::endl;

		sm->bindShader(ShaderManager::tempShadows2);
		glUniform1i(glGetUniformLocation(sm->getProgramID(), "intTex"), 0);
		glUniform1i(glGetUniformLocation(sm->getProgramID(), "tex"), 1);
	}

Uint64 previousTime2 = 0;
int frames = 0;
float timeSinceMeasure = 0;
float test = 0;
void RenderSystem::update()
{
	// GLuint err;

	// make fps measurement. this is for testing
	Uint64 currentTime = SDL_GetPerformanceCounter();
	Uint64 freq = SDL_GetPerformanceFrequency();
	float dt = ((currentTime - previousTime2) / (float)freq);
	// std::cout << "FPS: " << 1.0 / dt << std::endl;
	previousTime2 = currentTime;
	frames++;
	timeSinceMeasure += dt;
	if(timeSinceMeasure > 5.0)
	{
		std::cout << "FPS: " << frames / timeSinceMeasure << std::endl;
		frames = 0;
		timeSinceMeasure = 0;
	}

	//make list of all lights in the scene
	std::vector<int> lightList;
	int limit = lights->getSize();
	for(int i = 0; i < limit; i++)
	{
		Light * l = lights->getComponent(i);
		if(l)
			lightList.push_back(i);
	}

	glViewport(0,0,textureWidth, textureHeight);

	deferredShadingData.bindFrameBuffer();
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	test += .5 * dt;

	Player * p = player->getComponent(cameraID);
	Transform * pLoc = transforms->getComponent(cameraID);
	if(p == NULL)
	{
		std::cout << "COULD NOT FIND PLAYER COMPONENT" << std::endl;
		//determine new cameraID
	}

	//calculate where to place the camera in order to make the view matrix
	glm::vec3 cameraLoc = p->cameraOffset * glm::vec3(sin(p->cameraYaw) * cos(p->cameraPitch),
									sin(p->cameraPitch),
									cos(p->cameraYaw) * cos(p->cameraPitch))
						  + glm::vec3(pLoc->position);
	glm::vec3 up = glm::vec3(0.0,1.0,0.0);
	glm::mat4 view = glm::lookAt(cameraLoc,glm::vec3(pLoc->position),up);
	drawAllRenderables( &view, &projection );

	// Create per-light shadow maps
	renderShadowMaps();

	Test shadow map display
	// shaders->bindShader(ShaderManager::tempShadows);
	// Light *testLight = lights->getComponent( lightList[3] );
	// glActiveTexture(GL_TEXTURE0);
	// glBindTexture(GL_TEXTURE_2D, testLight->shadowMapTextures[1]);
	// glBindFramebuffer(GL_FRAMEBUFFER, 0);
	// glViewport(0,0,windowWidth, windowHeight);
	// renderFullScreenQuad();
	// return;

	glViewport(0,0,textureWidth, textureHeight);

	// Pingpong between lights, filling in integer shadow texture
	shaders->bindShader(ShaderManager::testShadows);
	shadowTestBuffer[0].bindFrameBuffer();
	glClear(GL_COLOR_BUFFER_BIT);
	shadowTestBuffer[1].bindFrameBuffer();
	glClear(GL_COLOR_BUFFER_BIT);
	bool bufferIndex = 0;
	for( int i=0; i<lightList.size(); i++ ) {
		bufferIndex = !bufferIndex;
		testSingleLight(lightList[i], i, bufferIndex, &view);
	}

	// // Test combined shadow texture	
	// shaders->bindShader(ShaderManager::tempShadows2);
	// shadowTestBuffer[bufferIndex].bindTexture(shadowTestTexture[bufferIndex], GL_TEXTURE0);
	// shadowTestBuffer[bufferIndex].bindTexture(shadowTempTexture[bufferIndex], GL_TEXTURE1);
	// glBindFramebuffer(GL_FRAMEBUFFER, 0);
	// glViewport(0,0,windowWidth,windowHeight);
	// renderFullScreenQuad();
	// while( (err = glGetError()) != GL_NO_ERROR ) {int ner = 0; std::cout << "Regs " << ner++ << " - " << err << std::endl;}
	// return;


	// perform shading pass
	shaders->bindShader(ShaderManager::shadingPass);
	glUniform3f(cameraPositionUniformLoc, cameraLoc.x, cameraLoc.y, cameraLoc.z);
	
	loadLights(&lightList);
	shadingTarget.bindFrameBuffer();
	deferredShadingData.bindTexture(positionTexture, GL_TEXTURE0);
	deferredShadingData.bindTexture(normalTexture, GL_TEXTURE1);
	deferredShadingData.bindTexture(diffuseTexture, GL_TEXTURE2);
	deferredShadingData.bindTexture(emissiveTexture, GL_TEXTURE3);
	shadowTestBuffer[bufferIndex].bindTexture(shadowTestTexture[bufferIndex], GL_TEXTURE4);
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glEnable(GL_DEPTH_TEST);
	renderFullScreenQuad();



	bloomBuffer[0].bindFrameBuffer();
	shaders->bindShader(ShaderManager::blur);
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	//deferredShadingData.bindTexture(emissiveTexture, GL_TEXTURE0);
	int pingpong = 0;

	// blur bright texture for bloom effect
	for(int i = 0; i < BLUR_PASSES; i++)
	{

		bloomBuffer[pingpong].bindFrameBuffer();
		shaders->bindShader(ShaderManager::blur);
		glUniform1i(horizontalBoolLoc, pingpong);
		glClear( GL_COLOR_BUFFER_BIT );
		if(i == 0)
			shadingTarget.bindTexture(brightTexture, GL_TEXTURE0);
		else bloomBuffer[!pingpong].bindTexture(bloomTexture[!pingpong], GL_TEXTURE0);
		renderFullScreenQuad();
		pingpong = !pingpong;
	}

	// apply the bloom to the color texture created by the shading pass
	shaders->bindShader(ShaderManager::applyBloom);
	bloomTarget.bindFrameBuffer();
	glClear( GL_COLOR_BUFFER_BIT );
	shadingTarget.bindTexture(colorTexture, GL_TEXTURE0);
	bloomBuffer[!pingpong].bindTexture(bloomTexture[!pingpong], GL_TEXTURE1);
	renderFullScreenQuad();


	//apply HDR and render to the screen
	glViewport(0,0,windowWidth, windowHeight);
	shaders->bindShader(ShaderManager::HDR);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUniform1f(exposureLoc, 2.0);
	bloomTarget.bindTexture(finalColorTexture, GL_TEXTURE0);
	//bloomBuffer[!pingpong].bindTexture(bloomTexture[!pingpong], GL_TEXTURE0);
	//deferredShadingData.bindTexture(emissiveTexture, GL_TEXTURE0);
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	//glEnable(GL_DEPTH_TEST);
	renderFullScreenQuad();

	//bloomBuffer[!pingpong].bindTexture(bloomTexture[!pingpong], GL_TEXTURE0);
	//renderFullScreenQuad();

	//glDisable(GL_BLEND);



	// // debugging test render normals to double check correctness
	// glBindFramebuffer(GL_FRAMEBUFFER, 0);
	// shaders->bindShader(4);
	// deferredShadingData.bindTexture(normalTexture, GL_TEXTURE0);
	// glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	// // glEnable(GL_DEPTH_TEST);
	// renderFullScreenQuad();

	glFlush();
}

void RenderSystem::receiveMessage(BasicMessage * message)
{

}

void RenderSystem::reshape( int width, int height ) {
	//float asp = width / (float)height;
	windowWidth = width;
	windowHeight = height;
	glViewport(0,0,width,height);
	projection = glm::perspective(glm::radians(fov), width / (float)height , 0.1f, 100.f);
}

void RenderSystem::drawAllRenderables( glm::mat4 *viewMat, glm::mat4 *projMat, bool vertex_only ) {
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	// render all solid objects
	int count = renderables->getSize();
	for(int i = 0; i < count; i++)
	{
		Renderable * currentR = renderables->getComponent(i);
		Transform * currentT = transforms->getComponent(i);
		
		if(currentR && currentT)
		{
			//create model matrix
			glm::mat4 model = glm::scale(glm::mat4(1.0f), glm::vec3(currentT->scale));
			model = glm::rotate(model, currentT->orientation.x, xAxis);
			model = glm::rotate(model, currentT->orientation.y, yAxis);
			model = glm::rotate(model, currentT->orientation.z, zAxis);
			model = glm::translate(model, glm::vec3(currentT->position)/currentT->position.w);

			// bind program and uniforms, then draw matrix
			if( !vertex_only ) {
				glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(model)));

				shaders->bindShader(currentR->program);
				shaders->bindMaterial(&(currentR->material));
				shaders->loadNormalMatrix(&normalMatrix);
			}
			shaders->loadModelMatrix(&model);
			shaders->loadViewMatrix(viewMat);
			shaders->loadProjectionMatrix(projMat);

			glEnableVertexAttribArray( VERTEX_ATTRIB );
			glEnableVertexAttribArray( NORM_ATTRIB );
			if( !vertex_only ) {
				glEnableVertexAttribArray( RGBA_ATTRIB );
				glEnableVertexAttribArray( TAN_ATTRIB );
				glEnableVertexAttribArray( BITAN_ATTRIB );
				glEnableVertexAttribArray( UV_ATTRIB );
			}

			glBindBuffer( GL_ARRAY_BUFFER, currentR->VBO );
			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, currentR->IBO );
			glVertexAttribPointer( VERTEX_ATTRIB, 4, GL_FLOAT, GL_FALSE, 15 * sizeof(GLfloat), (GLvoid*)(0) );
			glVertexAttribPointer( NORM_ATTRIB,   3, GL_FLOAT, GL_FALSE, 15 * sizeof(GLfloat), (GLvoid*)(6 *sizeof(GLfloat)) );
			if( !vertex_only ) {
				glVertexAttribPointer( RGBA_ATTRIB,   4, GL_FLOAT, GL_FALSE, 15 * sizeof(GLfloat), (GLvoid*)(0 *sizeof(GLfloat)) );
				glVertexAttribPointer( TAN_ATTRIB,    3, GL_FLOAT, GL_FALSE, 15 * sizeof(GLfloat), (GLvoid*)(9 *sizeof(GLfloat)) );
				glVertexAttribPointer( BITAN_ATTRIB,  3, GL_FLOAT, GL_FALSE, 15 * sizeof(GLfloat), (GLvoid*)(12*sizeof(GLfloat)) );
				glVertexAttribPointer( UV_ATTRIB,     2, GL_FLOAT, GL_FALSE, 15 * sizeof(GLfloat), (GLvoid*)(4 *sizeof(GLfloat)) );
			}
			glDrawElements( GL_TRIANGLES, currentR->numVertices, GL_UNSIGNED_INT, 0 );

			glDisableVertexAttribArray( VERTEX_ATTRIB );
			glDisableVertexAttribArray( NORM_ATTRIB );
			if( !vertex_only ) {
				glDisableVertexAttribArray( RGBA_ATTRIB );
				glDisableVertexAttribArray( TAN_ATTRIB );
				glDisableVertexAttribArray( BITAN_ATTRIB );
				glDisableVertexAttribArray( UV_ATTRIB );
			}
		}
	}

	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
}

void RenderSystem::renderFullScreenQuad()
{
	glBindBuffer(GL_ARRAY_BUFFER, fullScreenVBO);
	glEnableVertexAttribArray( VERTEX_ATTRIB_2D );
	glEnableVertexAttribArray( UV_ATTRIB_2D );

	glVertexAttribPointer( VERTEX_ATTRIB_2D, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(0) );
	glVertexAttribPointer( UV_ATTRIB_2D,     2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3*sizeof(GLfloat)) );
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableVertexAttribArray( VERTEX_ATTRIB_2D );
	glDisableVertexAttribArray( UV_ATTRIB_2D );
}

void RenderSystem::renderShadowMaps() {
	//make list of all lights in the scene
	std::vector<int> lightList;
	for( int i=0; i<lights->getSize(); i++ ) {
		Light * l = lights->getComponent(i);
		if(l) lightList.push_back(i);
	}

	glViewport(0,0,SHADOW_MAP_DIMENSION,SHADOW_MAP_DIMENSION);
	shaders->bindShader(ShaderManager::shadows);

	// Render all solid objects from the perspective of all lights to various shadow maps
	for( int li=0; li<lightList.size(); li++ ) {
		Light *currentLight = lights->getComponent(lightList[li]);
		glm::vec4 currentTransform4 = transforms->getComponent(lightList[li])->position;
		glm::vec3 currentTransform = glm::vec3(currentTransform4) / currentTransform4.w;
		glm::vec3 lightLoc = currentLight->location + currentTransform;

		for( int smi=0; smi<6; smi++ ) {
			// Bind buffer and appropriate shadow map
			shadowMapBuffer.setDepthOnlyTexture( currentLight->shadowMapTextures[smi] );
			glClear(GL_DEPTH_BUFFER_BIT);
			glm::vec3 curUp;
			if( smi == 2 ) curUp = glm::vec3(0.0, 0.0, 1.0);
			else if( smi == 3 ) curUp = glm::vec3(0.0, 0.0, -1.0);
			else curUp = glm::vec3(0.0, 1.0, 0.0);
			glm::mat4 view = glm::lookAt(lightLoc, lightLoc + lightViews[smi], curUp);

			drawAllRenderables( &view, &lightProjection, true );
		}
	}
}

void RenderSystem::testSingleLight( int componentIndex, int lightIndex, bool bufferIndex, glm::mat4 *viewMat ) {
	// Bind current framebuffer and shadow texture
	shadowTestBuffer[bufferIndex].bindFrameBuffer();
	shadowTestBuffer[!bufferIndex].bindTexture(shadowTestTexture[!bufferIndex], GL_TEXTURE1);

	// Bind uniforms for current light
	Light *l = lights->getComponent(componentIndex);
	glm::vec4 currentTransform4 = transforms->getComponent(componentIndex)->position;
	glm::vec3 currentTransform = glm::vec3(currentTransform4) / currentTransform4.w;
	glm::vec3 lightLoc = l->location + currentTransform;
	glUniform1i(shadowTestLightIndexLoc, lightIndex);
	glUniform3f(shadowTestLightLocLoc, lightLoc.x, lightLoc.y, lightLoc.z);
	glUniform2f(shadowTestWindowSizeLoc, textureWidth, textureHeight);

	// Bind view matrices and depth maps for all six views
	for( int i=0; i<6; i++ ) {
		glm::vec3 curUp;
		if( i == 2 ) curUp = glm::vec3(0.0, 0.0, 1.0);
		else if( i == 3 ) curUp = glm::vec3(0.0, 0.0, -1.0);
		else curUp = glm::vec3(0.0, 1.0, 0.0);
		glm::mat4 curView = glm::lookAt(lightLoc, lightLoc + lightViews[i], curUp);
		glUniformMatrix4fv(shadowTestLightViewLocs[i], 1, GL_FALSE, glm::value_ptr(curView));
		glActiveTexture(GL_TEXTURE2 + i);
		glBindTexture(GL_TEXTURE_2D, l->shadowMapTextures[i]);
	}

	// Test
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	drawAllRenderables( viewMat, &projection, true );
}

void RenderSystem::setUpFrameBuffers()
{
	// add textures to deferred shading frame buffer
	positionTexture = deferredShadingData.addTexture(textureWidth,textureHeight);
	normalTexture = deferredShadingData.addTexture(textureWidth,textureHeight);
	diffuseTexture = deferredShadingData.addTexture(textureWidth,textureHeight);
	emissiveTexture = deferredShadingData.addTexture(textureWidth,textureHeight);

	deferredShadingData.addDepthBuffer(textureWidth,textureHeight);

	//set up bloom pingpong textures
	bloomTexture[0] = bloomBuffer[0].addTexture(textureWidth,textureHeight);
	bloomTexture[1] = bloomBuffer[1].addTexture(textureWidth,textureHeight);

	//set up shading target textures
	colorTexture = shadingTarget.addTexture(textureWidth, textureHeight);
	brightTexture = shadingTarget.addTexture(textureWidth,textureHeight);

	//set up textures for bloomTarget, which is the final framebuffer before rendering to the screen
	finalColorTexture = bloomTarget.addTexture(textureWidth, textureHeight);

	// Set up shadow test buffers
	shadowTestTexture[0] = shadowTestBuffer[0].addIntegerTexture(textureWidth,textureHeight);
	shadowTestTexture[1] = shadowTestBuffer[1].addIntegerTexture(textureWidth,textureHeight);
	shadowTempTexture[0] = shadowTestBuffer[0].addTexture(textureWidth,textureHeight);
	shadowTempTexture[1] = shadowTestBuffer[1].addTexture(textureWidth,textureHeight);
	shadowTestBuffer[0].addDepthBuffer(textureWidth, textureHeight);
	shadowTestBuffer[1].addDepthBuffer(textureWidth, textureHeight);
}

void RenderSystem::loadLights(std::vector<int> *lightList)
{
	for(int i = 0; i < lightList->size() && i < MAX_LIGHTS; i++)
	{
		int entityID = lightList->at(i);
		Transform * t = transforms->getComponent(entityID);
		Light * l = lights->getComponent(entityID);
		shaders->loadLight(l,t,i);
	}
	shaders->loadLightCount(lightList->size());
}

