#include "RenderSystem.h"

Uint64 previousTime2 = 0;
int frames = 0;
float timeSinceMeasure = 0;
float test = 0;
void RenderSystem::update()
{
	Uint64 currentTime = SDL_GetPerformanceCounter();
	Uint64 freq = SDL_GetPerformanceFrequency();
	float dt = ((currentTime - previousTime2) / (float)freq);
	//std::cout << "FPS: " << 1.0 / dt << std::endl;
	previousTime2 = currentTime;
	frames++;
	timeSinceMeasure += dt;
	if(timeSinceMeasure > 1.0)
	{
		std::cout << "FPS: " << frames / timeSinceMeasure << std::endl;
		frames = 0;
		timeSinceMeasure = 0;
	}


	glViewport(0,0,textureWidth, textureHeight);
	deferredShadingData.bindFrameBuffer();
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	test += .5 * dt;
	//std::cout << "updating rendersystem: "<< renderables->getSize() << std::endl;
	//ComponentManager<int> test = ComponentManager<int>();
	Player * p = player->getComponent(cameraID);
	Transform * pLoc = transforms->getComponent(cameraID);
	if(p == NULL)
	{
		std::cout << "COULD NOT FIND PLAYER COMPONENT" << std::endl;
		//determine new cameraID
	}

	glm::vec3 cameraLoc = glm::vec3(sin(p->cameraYaw+test) * (cos(p->cameraPitch)*p->cameraOffset),
									sin(p->cameraPitch)*p->cameraOffset,
									cos(p->cameraYaw+test) * (cos(p->cameraPitch)*p->cameraOffset))
									 + pLoc->position.xyz();
	glm::vec3 up = glm::vec3(0.0,1.0,0.0);
	glm::mat4 view = glm::lookAt(cameraLoc,pLoc->position.xyz(),up);

	glm::vec3 xAxis = glm::vec3(1.0,0.0,0.0);
	glm::vec3 yAxis = glm::vec3(0.0,1.0,0.0);
	glm::vec3 zAxis = glm::vec3(0.0,0.0,1.0);


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


	shaders->bindShader(1);
	glUniform3f(cameraPositionUniformLoc, cameraLoc.x, cameraLoc.y, cameraLoc.z);
	// std::cout << cameraLoc.x<< ", " <<cameraLoc.y << ", " << cameraLoc.z << std::endl;
	HDRBuffer.bindFrameBuffer();
	//glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//shaders->bindShader(2);
	deferredShadingData.bindTexture(positionTexture, GL_TEXTURE0);
	deferredShadingData.bindTexture(normalTexture, GL_TEXTURE1);
	deferredShadingData.bindTexture(diffuseTexture, GL_TEXTURE2);
	deferredShadingData.bindTexture(emissiveTexture, GL_TEXTURE3);
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glEnable(GL_DEPTH_TEST);
	renderFullScreenQuad();

	glViewport(0,0,windowWidth, windowHeight);
	shaders->bindShader(3);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUniform1f(exposureLoc, 5.0);
	HDRBuffer.bindTexture(HDRColorTexture, GL_TEXTURE0);
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glEnable(GL_DEPTH_TEST);
	renderFullScreenQuad();


	//debugging test render normals to double check correctness
	// glBindFramebuffer(GL_FRAMEBUFFER, 0);
	// shaders->bindShader(2);
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

	HDRColorTexture = HDRBuffer.addTexture(textureWidth, textureHeight);
}

