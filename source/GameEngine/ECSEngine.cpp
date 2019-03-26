#include "ECSEngine.h"

ECSEngine::ECSEngine(ShaderManager * shaders_in)
{
	shaders = shaders_in;
	std::cout << "Creating Component Managers" << std::endl;
	std::cout << "Creating Systems" << std::endl;
	rendersystemObject = RenderSystem(&messenger, shaders, &transformManager, &renderableManager, &playerManager);
	std::cout << "Successfully Created Systems" << std::endl;
}


void ECSEngine::update()
{
	rendersystemObject.update();
}

void ECSEngine::spawnMessage(BasicMessage* message)
{
	messenger.sendMessage(message);
}

int ECSEngine::addEntity()
{
	return entities.createEntity();
}

void ECSEngine::addTransform(int entityID, Transform &comp)
{
	transformManager.addComponent(entityID, comp);
}

void ECSEngine::addRenderable(int entityID, Renderable &comp)
{
	renderableManager.addComponent(entityID, comp);
}

void ECSEngine::addPlayer(int entityID, Player &comp)
{
	playerManager.addComponent(entityID, comp);
}

RenderSystem* ECSEngine::getRenderSystem()
{
	return &rendersystemObject;
}
