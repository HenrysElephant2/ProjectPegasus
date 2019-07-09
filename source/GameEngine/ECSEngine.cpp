#include "ECSEngine.h"

ECSEngine::ECSEngine()
{
	shaders = ShaderManager::createShaderManager();
	std::cout << "Creating Component Managers" << std::endl;
	std::cout << "Creating Systems" << std::endl;
	// rendersystemObject = RenderSystem(&messenger, shaders, &transformManager, &renderableManager, &playerManager, &lightManager);
	playerMovementSystemObject = PlayerMovementSystem(&messenger, &transformManager, &playerManager);
	messenger.subscribe((System*)(&playerMovementSystemObject),KEY_EVENT);
	messenger.subscribe((System*)(&playerMovementSystemObject),MOUSE_EVENT);
	rendersystemObject = RenderSystem(&messenger, &transformManager, &renderableManager, &skinnedManager, &playerManager, &lightManager, &particleManager);
	animationHandler = AnimationSystem(&messenger, &skinnedManager);
	std::cout << "Successfully Created Systems" << std::endl;
}


void ECSEngine::update()
{
	
	playerMovementSystemObject.update();
	animationHandler.update();
	rendersystemObject.update();
	
}

void ECSEngine::spawnMessage(BasicMessage* message)
{
	messenger.sendMessage(message);
}

int ECSEngine::addEntity()
{
	int entityID = entities.createEntity();
	Transform defaultTransform = Transform();
	defaultTransform.ownerID = entityID;
	addTransform(entityID,defaultTransform);
	return entityID;
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

void ECSEngine::addLight(int entityID, Light &comp)
{
	lightManager.addComponent(entityID, comp);
}

void ECSEngine::addSkinnedRenderable(int entityID, SkinnedRenderable & comp)
{
	skinnedManager.addComponent(entityID, comp);
}

void ECSEngine::addParticleSystem(int entityID, ParticleSystem &comp)
{
	particleManager.addComponent(entityID, comp);
}

ComponentManager<Transform> * ECSEngine::getTransformManager(){return &transformManager;}
ComponentManager<Renderable> * ECSEngine::getRenderableManager(){return &renderableManager;}
ComponentManager<Player> * ECSEngine::getPlayerManager(){return &playerManager;}
ComponentManager<Light> * ECSEngine::getLightManager(){return &lightManager;}
ComponentManager<SkinnedRenderable> * ECSEngine::getSkinnedRenderableManager(){return &skinnedManager;}
ComponentManager<ParticleSystem> * ECSEngine::getParticleSystemManager(){return &particleManager;}

RenderSystem* ECSEngine::getRenderSystem()
{
	return &rendersystemObject;
}
