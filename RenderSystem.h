#ifndef RENDER_SYSTEM_H
#define RENDER_SYSTEM_H

#define GLM_SWIZZLE
#include <glm/glm.hpp>
#include "ComponentManager.h"
#include "Component.h"
#include "System.h"
#include "MessageManager.h"
#include "ShaderManager.h"


#define VERTEX_ATTRIB 0
#define RGBA_ATTRIB 1
#define NORM_ATTRIB 2
#define TAN_ATTRIB 3
#define BITAN_ATTRIB 4
#define UV_ATTRIB 5


class RenderSystem:System {
private:
	ComponentManager<Transform> * transforms;
	ComponentManager<Renderable> * renderables;
	ComponentManager<Player> * player;
	ShaderManager * shaders;
	int cameraID = 0;
	glm::mat4 projection;
	float fov = 55;

	int windowWidth, windowHeight;


public:
	RenderSystem(MessageManager * m, ShaderManager * sm, ComponentManager<Transform> * transforms_in, ComponentManager<Renderable> * renderables_in, ComponentManager<Player> * player_in ):System(m)
	{
		shaders = sm;
		transforms = transforms_in;
		renderables = renderables_in;
		player = player_in;
		std::cout << "Created RenderSystem" << std::endl;
	}
	RenderSystem():System(NULL){}
	void update();
	void receiveMessage(BasicMessage * message);
	void reshape(int width, int height);
};

#endif