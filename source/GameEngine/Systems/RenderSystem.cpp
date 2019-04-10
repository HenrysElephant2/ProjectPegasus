#include "RenderSystem.h"

Uint64 previousTime2 = 0;
int frames = 0;
float timeSinceMeasure = 0;
float test = 0;
void RenderSystem::update()
{
	// make fps measurement. this is for testing
	Uint64 currentTime = SDL_GetPerformanceCounter();
	Uint64 freq = SDL_GetPerformanceFrequency();
	float dt = ((currentTime - previousTime2) / (float)freq);
	//std::cout << "FPS: " << 1.0 / dt << std::endl;
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
	//glViewport(0,0,windowWidth, windowHeight);
	deferredShadingData.bindFrameBuffer();
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	test += .5 * dt;

	Player * p = player->getComponent(cameraID);
	Transform * pLoc = transforms->getComponent(cameraID);
	if(p == NULL)
	{
		std::cout << "COULD NOT FIND PLAYER COMPONENT" << std::endl;
		//determine new cameraID
	}

	//calculate where to place the camera in order to make the view matrix
	glm::vec3 cameraLoc = glm::vec3(sin(p->cameraYaw+test) * (cos(p->cameraPitch)*p->cameraOffset),
									sin(p->cameraPitch)*p->cameraOffset,
									cos(p->cameraYaw+test) * (cos(p->cameraPitch)*p->cameraOffset))
									 + pLoc->position.xyz();
	glm::vec3 up = glm::vec3(0.0,1.0,0.0);
	glm::mat4 view = glm::lookAt(cameraLoc,pLoc->position.xyz(),up);

	glm::vec3 xAxis = glm::vec3(1.0,0.0,0.0);
	glm::vec3 yAxis = glm::vec3(0.0,1.0,0.0);
	glm::vec3 zAxis = glm::vec3(0.0,0.0,1.0);


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
			model = glm::translate(model, currentT->position.xyz()/currentT->position.w);

			glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(model)));

			// bind program and uniforms, then draw matrix
			shaders->bindShader(currentR->program);
			shaders->bindMaterial(&(currentR->material));
			shaders->loadModelMatrix(&model);
			shaders->loadNormalMatrix(&normalMatrix);
			shaders->loadViewMatrix(&view);
			shaders->loadProjectionMatrix(&projection);

			glEnableVertexAttribArray( VERTEX_ATTRIB );
			glEnableVertexAttribArray( RGBA_ATTRIB );
			glEnableVertexAttribArray( NORM_ATTRIB );
			glEnableVertexAttribArray( TAN_ATTRIB );
			glEnableVertexAttribArray( BITAN_ATTRIB );
			glEnableVertexAttribArray( UV_ATTRIB );

			glBindBuffer( GL_ARRAY_BUFFER, currentR->VBO );
			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, currentR->IBO );
			glVertexAttribPointer( VERTEX_ATTRIB, 4, GL_FLOAT, GL_FALSE, 15 * sizeof(GLfloat), (GLvoid*)(0) );
			glVertexAttribPointer( RGBA_ATTRIB,   4, GL_FLOAT, GL_FALSE, 15 * sizeof(GLfloat), (GLvoid*)(0 *sizeof(GLfloat)) );
			glVertexAttribPointer( NORM_ATTRIB,   3, GL_FLOAT, GL_FALSE, 15 * sizeof(GLfloat), (GLvoid*)(6 *sizeof(GLfloat)) );
			glVertexAttribPointer( TAN_ATTRIB,    3, GL_FLOAT, GL_FALSE, 15 * sizeof(GLfloat), (GLvoid*)(9*sizeof(GLfloat)) );
			glVertexAttribPointer( BITAN_ATTRIB,  3, GL_FLOAT, GL_FALSE, 15 * sizeof(GLfloat), (GLvoid*)(12*sizeof(GLfloat)) );
			glVertexAttribPointer( UV_ATTRIB,     2, GL_FLOAT, GL_FALSE, 15 * sizeof(GLfloat), (GLvoid*)(4*sizeof(GLfloat)) );
			glDrawElements( GL_TRIANGLES, currentR->numVertices, GL_UNSIGNED_INT, 0 );

			glDisableVertexAttribArray( VERTEX_ATTRIB );
			glDisableVertexAttribArray( RGBA_ATTRIB );
			glDisableVertexAttribArray( NORM_ATTRIB );
			glDisableVertexAttribArray( TAN_ATTRIB );
			glDisableVertexAttribArray( BITAN_ATTRIB );
			glDisableVertexAttribArray( UV_ATTRIB );
		}
	}
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);



	// perform shading pass
	shaders->bindShader(1);
	glUniform3f(cameraPositionUniformLoc, cameraLoc.x, cameraLoc.y, cameraLoc.z);
	
	loadLights(&lightList);
	shadingTarget.bindFrameBuffer();
	deferredShadingData.bindTexture(positionTexture, GL_TEXTURE0);
	deferredShadingData.bindTexture(normalTexture, GL_TEXTURE1);
	deferredShadingData.bindTexture(diffuseTexture, GL_TEXTURE2);
	deferredShadingData.bindTexture(emissiveTexture, GL_TEXTURE3);
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glEnable(GL_DEPTH_TEST);
	renderFullScreenQuad();



	bloomBuffer[0].bindFrameBuffer();
	shaders->bindShader(4);
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	//deferredShadingData.bindTexture(emissiveTexture, GL_TEXTURE0);
	int pingpong = 0;

	// blur bright texture for bloom effect
	for(int i = 0; i < BLUR_PASSES; i++)
	{

		bloomBuffer[pingpong].bindFrameBuffer();
		shaders->bindShader(4);
		glUniform1i(horizontalBoolLoc, pingpong);
		glClear( GL_COLOR_BUFFER_BIT );
		if(i == 0)
			shadingTarget.bindTexture(brightTexture, GL_TEXTURE0);
		else bloomBuffer[!pingpong].bindTexture(bloomTexture[!pingpong], GL_TEXTURE0);
		renderFullScreenQuad();
		pingpong = !pingpong;
	}

	// apply the bloom to the color texture created by the shading pass
	shaders->bindShader(5);
	bloomTarget.bindFrameBuffer();
	glClear( GL_COLOR_BUFFER_BIT );
	shadingTarget.bindTexture(colorTexture, GL_TEXTURE0);
	bloomBuffer[!pingpong].bindTexture(bloomTexture[!pingpong], GL_TEXTURE1);
	renderFullScreenQuad();



	//apply HDR and render to the screen
	glViewport(0,0,windowWidth, windowHeight);
	shaders->bindShader(3);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUniform1f(exposureLoc, 5.0);
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

