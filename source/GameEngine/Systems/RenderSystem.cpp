#include "RenderSystem.h"

float test = 0;
void RenderSystem::update()
{
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glEnable(GL_DEPTH_TEST);
	test += .001;
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

			// bind program and uniforms, then draw matrix
			shaders->bindShader(currentR->program);
			shaders->bindMaterial(&(currentR->material));
			shaders->loadModelMatrix(&model);
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
	glDisable(GL_DEPTH_TEST);
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

