#include "RenderSystem.h"

glm::vec3 RenderSystem::lightViews[6] = {
	glm::vec3( 1.0, 0.0, 0.0),
	glm::vec3(-1.0, 0.0, 0.0),
	glm::vec3( 0.0, 1.0, 0.0),
	glm::vec3( 0.0,-1.0, 0.0),
	glm::vec3( 0.0, 0.0, 1.0),
	glm::vec3( 0.0, 0.0,-1.0)
};

RenderSystem::RenderSystem(MessageManager * m):System(m)
{
	shaders = ShaderManager::createShaderManager();

	EntityManager * em = EntityManager::getEntityManager();

	Transform t = Transform();
	transforms = em->getComponentManager(t);

	Renderable r = Renderable();
	renderables = em->getComponentManager(r);

	Player p = Player();
	playerManager = em->getComponentManager(p);

	SkinnedRenderable sr = SkinnedRenderable();
	skinnedRenderables = em->getComponentManager(sr);

	Light l = Light();
	lights = em->getComponentManager(l);

	ParticleSystem ps = ParticleSystem();
	particleSystems = em->getComponentManager(ps);


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

	shaders->bindShader(ShaderManager::shadingPass);
	cameraPositionUniformLoc = glGetUniformLocation(shaders->getProgramID(), "cameraLoc");
	glUniform1i(glGetUniformLocation(shaders->getProgramID(), "positionTexture"), 0);
	glUniform1i(glGetUniformLocation(shaders->getProgramID(), "normalTexture"), 1);
	glUniform1i(glGetUniformLocation(shaders->getProgramID(), "diffuseTexture"), 2);
	glUniform1i(glGetUniformLocation(shaders->getProgramID(), "emissiveTexture"), 3);
	glUniform1i(glGetUniformLocation(shaders->getProgramID(), "shadowTexture"), 4);

	// shaders->bindShader(ShaderManager::occlusionMapSetup);
	// depthLoc = glGetUniformLocation(shaders->getProgramID(), "depth");
	// lightScreenPositionLoc = glGetUniformLocation(shaders->getProgramID(), "lightScreenPosition");

	shaders->bindShader(ShaderManager::HDR);
	exposureLoc = glGetUniformLocation(shaders->getProgramID(), "exposure");

	shaders->bindShader(ShaderManager::blur);
	horizontalBoolLoc = glGetUniformLocation(shaders->getProgramID(), "horizontal");

	shaders->bindShader(ShaderManager::applyBloom);
	glUniform1i(glGetUniformLocation(shaders->getProgramID(), "baseColor"), 0);
	glUniform1i(glGetUniformLocation(shaders->getProgramID(), "bloomColor"), 1);


	shaders->bindShader(ShaderManager::volumetricLightScattering);
	sunScreenCoordLoc = glGetUniformLocation(shaders->getProgramID(), "sunScreenCoords");
	densityLoc = glGetUniformLocation(shaders->getProgramID(), "Density");
	weightLoc = glGetUniformLocation(shaders->getProgramID(), "Weight");
	decayLoc = glGetUniformLocation(shaders->getProgramID(), "Decay");


	shaders->bindShader(ShaderManager::testShadows);
	glUniformMatrix4fv( glGetUniformLocation(shaders->getProgramID(), "LightProjection"), 1, GL_FALSE, glm::value_ptr(lightProjection));
	shadowTestLightIndexLoc = glGetUniformLocation(shaders->getProgramID(), "LightIndex");
	shadowTestLightViewLocs[0] = glGetUniformLocation(shaders->getProgramID(), "LightViews[0]");
	shadowTestLightViewLocs[1] = glGetUniformLocation(shaders->getProgramID(), "LightViews[1]");
	shadowTestLightViewLocs[2] = glGetUniformLocation(shaders->getProgramID(), "LightViews[2]");
	shadowTestLightViewLocs[3] = glGetUniformLocation(shaders->getProgramID(), "LightViews[3]");
	shadowTestLightViewLocs[4] = glGetUniformLocation(shaders->getProgramID(), "LightViews[4]");
	shadowTestLightViewLocs[5] = glGetUniformLocation(shaders->getProgramID(), "LightViews[5]");
	shadowTestLightLocLoc = glGetUniformLocation(shaders->getProgramID(), "LightLoc");

	// Uniform texture locations
	glUniform1i(glGetUniformLocation(shaders->getProgramID(), "positionTexture"), 0);
	glUniform1i(glGetUniformLocation(shaders->getProgramID(), "normalTexture"), 1);
	glUniform1i(glGetUniformLocation(shaders->getProgramID(), "currentTex"), 2);
	glUniform1i(glGetUniformLocation(shaders->getProgramID(), "ShadowMap0"), 3);
	glUniform1i(glGetUniformLocation(shaders->getProgramID(), "ShadowMap1"), 4);
	glUniform1i(glGetUniformLocation(shaders->getProgramID(), "ShadowMap2"), 5);
	glUniform1i(glGetUniformLocation(shaders->getProgramID(), "ShadowMap3"), 6);
	glUniform1i(glGetUniformLocation(shaders->getProgramID(), "ShadowMap4"), 7);
	glUniform1i(glGetUniformLocation(shaders->getProgramID(), "ShadowMap5"), 8);

	shaders->bindShader(ShaderManager::testShadowsDirectional);
	glUniformMatrix4fv( glGetUniformLocation(shaders->getProgramID(), "LightProjection"), 1, GL_FALSE, glm::value_ptr(lightOrthoProjection));
	directionalShadowTestLightIndexLoc = glGetUniformLocation(shaders->getProgramID(), "LightIndex");
	directionalShadowTestLightViewLoc = glGetUniformLocation(shaders->getProgramID(), "LightView");
	directionalShadowTestLightLocLoc = glGetUniformLocation(shaders->getProgramID(), "LightLoc");
	// Uniform texture locations
	glUniform1i(glGetUniformLocation(shaders->getProgramID(), "positionTexture"), 0);
	glUniform1i(glGetUniformLocation(shaders->getProgramID(), "normalTexture"), 1);
	glUniform1i(glGetUniformLocation(shaders->getProgramID(), "currentTex"), 2);
	glUniform1i(glGetUniformLocation(shaders->getProgramID(), "ShadowMap"), 3);

	shaders->bindShader(ShaderManager::tempShadows2);
	glUniform1i(glGetUniformLocation(shaders->getProgramID(), "intTex"), 0);
	glUniform1i(glGetUniformLocation(shaders->getProgramID(), "tex"), 1);

	shaders->bindShader(ShaderManager::displayParticles);
	pTimeLoc = glGetUniformLocation(shaders->getProgramID(), "time");

	std::cout << "Created RenderSystem" << std::endl;
}


Uint64 previousTime2 = 0;
int frames = 0;
float timeSinceMeasure = 0;
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
	ptime += dt;
	if( ptime > 5.0 ) ptime = 0;

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

	Player * p = playerManager->getComponent(cameraID);
	Transform * pLoc = transforms->getComponent(cameraID);
	if(p == NULL)
	{
		std::cout << "COULD NOT FIND PLAYER COMPONENT" << std::endl;
		//determine new cameraID
		bool newPlayerFound = false;
		int i = 0;
		int count = playerManager->getSize();
		while(!newPlayerFound && i < count) {
			Player * current = playerManager->getComponent(i);

			if(current) {
				pLoc = transforms->getComponent(i);
				cameraID = i;
				p = current;
				newPlayerFound = true;
			}
			i++;
		}
	}

	//calculate where to place the camera in order to make the view matrix
	glm::vec3 cameraLoc = p->cameraOffset * glm::vec3(sin(p->cameraYaw) * cos(p->cameraPitch),
									sin(p->cameraPitch),
									cos(p->cameraYaw) * cos(p->cameraPitch))
						  + glm::vec3(pLoc->position);
	glm::vec3 up = glm::vec3(0.0,1.0,0.0);
	glm::mat4 view = glm::lookAt(cameraLoc,glm::vec3(pLoc->position),up);
	drawSkinnedRenderables( &view, &projection );
	drawAllRenderables( &view, &projection );
	drawParticleSystems( &view, &projection );


	// Create per-light shadow maps
	renderShadowMaps( glm::vec3(pLoc->position) );

	// // Test shadow map display
	// shaders->bindShader(ShaderManager::tempShadows);
	// Light *testLight = lights->getComponent( lightList[1] );
	// glActiveTexture(GL_TEXTURE0);
	// glBindTexture(GL_TEXTURE_2D, testLight->shadowMapTextures[0]);
	// glBindFramebuffer(GL_FRAMEBUFFER, 0);
	// glViewport(0,0,windowWidth, windowHeight);
	// renderFullScreenQuad();
	// return;

	glViewport(0,0,textureWidth, textureHeight);

	// Pingpong between lights, filling in integer shadow texture
	shadowTestBuffer[0].bindFrameBuffer();
	glClear(GL_COLOR_BUFFER_BIT);
	shadowTestBuffer[1].bindFrameBuffer();
	glClear(GL_COLOR_BUFFER_BIT);
	bool bufferIndex = 0;
	for( int i=0; i<lightList.size(); i++ ) {
		Light *currentLight = lights->getComponent(lightList[i]);
		bufferIndex = !bufferIndex;
		if( currentLight->directional )
			testSingleDirectionalLight(lightList[i], i, bufferIndex, glm::vec3(pLoc->position));
		else
			testSingleLight(lightList[i], i, bufferIndex);
	}

	// // Test combined shadow texture	
	// shaders->bindShader(ShaderManager::tempShadows2);
	// shadowTestBuffer[bufferIndex].bindTexture(shadowTestTexture[bufferIndex], GL_TEXTURE0);
	// shadowTestBuffer[bufferIndex].bindTexture(shadowTempTexture[bufferIndex], GL_TEXTURE1);
	// glBindFramebuffer(GL_FRAMEBUFFER, 0);
	// glViewport(0,0,windowWidth,windowHeight);
	// renderFullScreenQuad();
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


	glViewport(0,0,textureWidth, textureHeight);
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



	// apply volumetric light scattering post process
	
	// hardcoded sunID - NEEDS TO BE CHANGED
	if(lightList.size() > 1)
		sunID = lightList[1];

	Light * sun = lights->getComponent(sunID);
	Transform * sunLoc = transforms->getComponent(sunID);

	if(sun && sunLoc)
	{
		glm::vec4 lightScreenLoc;
		if(sun->directional)
			lightScreenLoc = projection * view * (glm::vec4(cameraLoc + sun->direction * -100.0f,1.0));
		else lightScreenLoc = projection * view * (glm::vec4(sun->location,0.0) + sunLoc->position / sunLoc->position.w);
		// only apply volumetric light scattering if looking towards the light emitting it
		if(lightScreenLoc.w > 0.0f)
		{
			//std::cout << "Light loc: " << lightScreenLoc.x << ", " << lightScreenLoc.y <<", " << lightScreenLoc.z << ", " << lightScreenLoc.w << std::endl;
			float wVal = lightScreenLoc.w;
			lightScreenLoc = lightScreenLoc / abs(lightScreenLoc.w);
			
			if(abs(lightScreenLoc.x) > 5 || abs(lightScreenLoc.y) > 5)
			{
				lightScreenLoc = glm::normalize(lightScreenLoc) * 6.0f;
			}
			lightScreenLoc += glm::vec4(1.0,1.0,0.0,0.0);
			lightScreenLoc = lightScreenLoc * .5f;
		
			shaders->bindShader(ShaderManager::volumetricLightScattering);
			bloomTarget.bindFrameBuffer();
			deferredShadingData.bindTexture(occlusionTexture, GL_TEXTURE0); // use occlusionTexture for actual volumetric light scattering
			
			//load uniforms
			glUniform2f(sunScreenCoordLoc, lightScreenLoc.x, lightScreenLoc.y);
			// linearly reduce weight of volumetric light scattering when looking almost perpendicular to lights
			if(wVal < 45.0)
				glUniform1f(weightLoc, fmaxf(wVal/1500.0f - .01f, 0.0f));
			else glUniform1f(weightLoc, .02);
			glUniform1f(densityLoc, .2);
			glUniform1f(decayLoc, .9);
			
		
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE);
			renderFullScreenQuad();
			glDisable(GL_BLEND);
		}
	}


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
	// deferredShadingData.bindTexture(occlusionTexture, GL_TEXTURE0);
	// glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	// // glEnable(GL_DEPTH_TEST);
	// renderFullScreenQuad();

	// glFlush();
	glFinish();
	currentTime = SDL_GetPerformanceCounter();

	//std::cout << "Time to render: " << (currentTime - previousTime2) / (float)freq << std::endl;

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

void RenderSystem::drawSkinnedRenderables(glm::mat4 *viewMat, glm::mat4 *projMat, bool vertex_only)
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	// render all solid objects
	int count = skinnedRenderables->getSize();
	for(int i = 0; i < count; i++)
	{
		SkinnedRenderable * currentR = skinnedRenderables->getComponent(i);
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

			shaders->loadBones(&currentR->bones);

			glEnableVertexAttribArray( VERTEX_ATTRIB );
			glEnableVertexAttribArray( NORM_ATTRIB );
			if( !vertex_only ) {
				glEnableVertexAttribArray( RGBA_ATTRIB );
				glEnableVertexAttribArray( TAN_ATTRIB );
				glEnableVertexAttribArray( BITAN_ATTRIB );
				glEnableVertexAttribArray( UV_ATTRIB );
			}
			glEnableVertexAttribArray(BONE_IDS_ATTRIB);
			glEnableVertexAttribArray(BONE_WEIGHTS_ATTRIB);

			glBindBuffer( GL_ARRAY_BUFFER, currentR->VBO );
			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, currentR->IBO );
			glVertexAttribPointer( VERTEX_ATTRIB, 4, GL_FLOAT, GL_FALSE, 19 * sizeof(GLfloat) + 4 * sizeof(GLint), (GLvoid*)(0) );
			glVertexAttribPointer( NORM_ATTRIB,   3, GL_FLOAT, GL_FALSE, 19 * sizeof(GLfloat) + 4 * sizeof(GLint), (GLvoid*)(6 *sizeof(GLfloat)) );
			if( !vertex_only ) {
				glVertexAttribPointer( RGBA_ATTRIB,   4, GL_FLOAT, GL_FALSE, 19 * sizeof(GLfloat) + 4 * sizeof(GLint), (GLvoid*)(0 *sizeof(GLfloat)) );
				glVertexAttribPointer( TAN_ATTRIB,    3, GL_FLOAT, GL_FALSE, 19 * sizeof(GLfloat) + 4 * sizeof(GLint), (GLvoid*)(9 *sizeof(GLfloat)) );
				glVertexAttribPointer( BITAN_ATTRIB,  3, GL_FLOAT, GL_FALSE, 19 * sizeof(GLfloat) + 4 * sizeof(GLint), (GLvoid*)(12*sizeof(GLfloat)) );
				glVertexAttribPointer( UV_ATTRIB,     2, GL_FLOAT, GL_FALSE, 19 * sizeof(GLfloat) + 4 * sizeof(GLint), (GLvoid*)(4 *sizeof(GLfloat)) );
			}
			glVertexAttribPointer(BONE_WEIGHTS_ATTRIB, 4, GL_FLOAT, GL_FALSE, 19 * sizeof(GLfloat) + 4 * sizeof(GLint), (GLvoid*)(15*sizeof(GLfloat)));
			glVertexAttribIPointer(BONE_IDS_ATTRIB, 4, GL_INT,  19 * sizeof(GLfloat) + 4 * sizeof(GLint), (GLvoid*)(19*sizeof(GLfloat)));
			glDrawElements( GL_TRIANGLES, currentR->numVertices, GL_UNSIGNED_INT, 0 );

			glDisableVertexAttribArray( VERTEX_ATTRIB );
			glDisableVertexAttribArray( NORM_ATTRIB );
			if( !vertex_only ) {
				glDisableVertexAttribArray( RGBA_ATTRIB );
				glDisableVertexAttribArray( TAN_ATTRIB );
				glDisableVertexAttribArray( BITAN_ATTRIB );
				glDisableVertexAttribArray( UV_ATTRIB );
			}
			glDisableVertexAttribArray(BONE_IDS_ATTRIB);
			glDisableVertexAttribArray(BONE_WEIGHTS_ATTRIB);
		}
	}

	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
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

void RenderSystem::renderShadowMaps( glm::vec3 playerLoc ) {
	//make list of all lights in the scene
	std::vector<int> lightList;
	for( int i=0; i<lights->getSize(); i++ ) {
		Light * l = lights->getComponent(i);
		if(l) lightList.push_back(i);
	}

	glViewport(0,0,SHADOW_MAP_DIMENSION,SHADOW_MAP_DIMENSION);
	

	// Render all solid objects from the perspective of all lights to various shadow maps
	for( int li=0; li<lightList.size(); li++ ) {
		Light *currentLight = lights->getComponent(lightList[li]);
		glm::vec4 currentTransform4 = transforms->getComponent(lightList[li])->position;
		glm::vec3 currentTransform = glm::vec3(currentTransform4) / currentTransform4.w;
		glm::vec3 lightLoc = currentLight->location + currentTransform;

		if( currentLight->directional ) {
			shadowMapBuffer.setDepthOnlyTexture( currentLight->shadowMapTextures[0] );
			glClear(GL_DEPTH_BUFFER_BIT);
			glm::vec3 curUp = glm::vec3(0.0, 1.0, 0.0);
			glm::mat4 view = glm::lookAt( playerLoc + glm::normalize(lightLoc), playerLoc, curUp);

			shaders->bindShader(ShaderManager::skinnedShadows);
			drawSkinnedRenderables( &view, &lightOrthoProjection, true );
			shaders->bindShader(ShaderManager::shadows);
			drawAllRenderables( &view, &lightOrthoProjection, true );
		}
		else {
			for( int smi=0; smi<6; smi++ ) {
				// Bind buffer and appropriate shadow map
				shadowMapBuffer.setDepthOnlyTexture( currentLight->shadowMapTextures[smi] );

				glClear(GL_DEPTH_BUFFER_BIT);
				glm::vec3 curUp = glm::vec3(0.0, 1.0, 0.0);
				if( smi == 2 || smi == 3 ) curUp = glm::vec3(0.0, 0.0, 1.0);
				glm::mat4 view = glm::lookAt(lightLoc, lightLoc + lightViews[smi], curUp);

				shaders->bindShader(ShaderManager::skinnedShadows);
				drawSkinnedRenderables( &view, &lightProjection, true );
				shaders->bindShader(ShaderManager::shadows);
				drawAllRenderables( &view, &lightProjection, true );
			}
		}
	}
}

void RenderSystem::testSingleLight( int componentIndex, int lightIndex, bool bufferIndex ) {
	shaders->bindShader(ShaderManager::testShadows);
	// Bind current framebuffer and shadow texture
	shadowTestBuffer[bufferIndex].bindFrameBuffer();
	deferredShadingData.bindTexture(positionTexture, GL_TEXTURE0);
	deferredShadingData.bindTexture(normalTexture, GL_TEXTURE1);
	shadowTestBuffer[!bufferIndex].bindTexture(shadowTestTexture[!bufferIndex], GL_TEXTURE2);

	// Bind uniforms for current light
	Light *l = lights->getComponent(componentIndex);
	glm::vec4 currentTransform4 = transforms->getComponent(componentIndex)->position;
	glm::vec3 currentTransform = glm::vec3(currentTransform4) / currentTransform4.w;
	glm::vec3 lightLoc = l->location + currentTransform;
	glUniform1i(shadowTestLightIndexLoc, lightIndex);
	glUniform3f(shadowTestLightLocLoc, lightLoc.x, lightLoc.y, lightLoc.z);

	// Bind view matrices and depth maps for all six views
	for( int i=0; i<6; i++ ) {
		glm::vec3 curUp = glm::vec3(0.0, 1.0, 0.0);
		if( i == 2 || i == 3 ) curUp = glm::vec3(0.0, 0.0, 1.0);
		glm::mat4 curView = glm::lookAt(lightLoc, lightLoc + lightViews[i], curUp);
		glUniformMatrix4fv(shadowTestLightViewLocs[i], 1, GL_FALSE, glm::value_ptr(curView));
		glActiveTexture(GL_TEXTURE3 + i);
		glBindTexture(GL_TEXTURE_2D, l->shadowMapTextures[i]);
	}

	// Test
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	renderFullScreenQuad();
}


void RenderSystem::testSingleDirectionalLight( int componentIndex, int lightIndex, bool bufferIndex, glm::vec3 playerLoc ) {
	shaders->bindShader(ShaderManager::testShadowsDirectional);
	// Bind current framebuffer and shadow texture
	shadowTestBuffer[bufferIndex].bindFrameBuffer();
	deferredShadingData.bindTexture(positionTexture, GL_TEXTURE0);
	deferredShadingData.bindTexture(normalTexture, GL_TEXTURE1);
	shadowTestBuffer[!bufferIndex].bindTexture(shadowTestTexture[!bufferIndex], GL_TEXTURE2);

	// Bind uniforms for current light
	Light *l = lights->getComponent(componentIndex);
	glm::vec4 currentTransform4 = transforms->getComponent(componentIndex)->position;
	glm::vec3 currentTransform = glm::vec3(currentTransform4) / currentTransform4.w;
	glm::vec3 lightLoc = l->location + currentTransform;
	glUniform1i(directionalShadowTestLightIndexLoc, lightIndex);
	glUniform3f(directionalShadowTestLightLocLoc, lightLoc.x, lightLoc.y, lightLoc.z);

	glm::vec3 curUp = glm::vec3(0.0, 1.0, 0.0);
	glm::mat4 curView = glm::lookAt(playerLoc + glm::normalize(lightLoc), playerLoc, curUp);
	glUniformMatrix4fv(directionalShadowTestLightViewLoc, 1, GL_FALSE, glm::value_ptr(curView));
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, l->shadowMapTextures[0]);

	// Test
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	renderFullScreenQuad();
}

void RenderSystem::drawParticleSystems( glm::mat4 *viewMat, glm::mat4 *projMat ) {
	glPointSize(3.0);
	int count = particleSystems->getSize();
	for( int i=0; i<count; i++ ) {
		ParticleSystem *currentPS = particleSystems->getComponent(i);

		if( currentPS ) {
			shaders->bindShader(currentPS->program);
			glUniform1f(pTimeLoc, ptime);
			glEnable(GL_DEPTH_TEST);
			glDepthMask(GL_FALSE);

			glm::mat4 model = glm::mat4(1.0f);
			shaders->loadModelMatrix(&model);
			shaders->loadViewMatrix(viewMat);
			shaders->loadProjectionMatrix(projMat);

			glBindBuffer(GL_ARRAY_BUFFER, currentPS->VBO);
			glEnableVertexAttribArray( VERTEX_ATTRIB );
			glEnableVertexAttribArray( RGBA_ATTRIB );
			glEnableVertexAttribArray( NORM_ATTRIB );
			glEnableVertexAttribArray( UV_ATTRIB );

			glVertexAttribPointer( VERTEX_ATTRIB, 3, GL_FLOAT, GL_FALSE, 12 * sizeof(GLfloat), (GLvoid*)(0) );
			glVertexAttribPointer( RGBA_ATTRIB,   4, GL_FLOAT, GL_FALSE, 12 * sizeof(GLfloat), (GLvoid*)(3*sizeof(GLfloat)) );
			glVertexAttribPointer( NORM_ATTRIB,   3, GL_FLOAT, GL_FALSE, 12 * sizeof(GLfloat), (GLvoid*)(7*sizeof(GLfloat)) );
			glVertexAttribPointer( UV_ATTRIB,     2, GL_FLOAT, GL_FALSE, 12 * sizeof(GLfloat), (GLvoid*)(10*sizeof(GLfloat)) );
			glDrawArrays(GL_POINTS, 0, currentPS->nParticles);

			glDisableVertexAttribArray( VERTEX_ATTRIB );
			glDisableVertexAttribArray( RGBA_ATTRIB );
			glDisableVertexAttribArray( NORM_ATTRIB );
			glEnableVertexAttribArray( UV_ATTRIB );

			glDepthMask(GL_TRUE);
			glDisable( GL_DEPTH_TEST );
		}
	}
}

void RenderSystem::setUpFrameBuffers()
{
	// add textures to deferred shading frame buffer
	positionTexture = deferredShadingData.addTexture(textureWidth,textureHeight);
	normalTexture = deferredShadingData.addTexture(textureWidth,textureHeight);
	diffuseTexture = deferredShadingData.addTexture(textureWidth,textureHeight);
	emissiveTexture = deferredShadingData.addTexture(textureWidth,textureHeight);
	occlusionTexture = deferredShadingData.addTexture(textureWidth,textureHeight);

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

