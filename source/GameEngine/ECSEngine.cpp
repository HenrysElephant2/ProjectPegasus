#include "ECSEngine.h"

ECSEngine::ECSEngine()
{
	entities = EntityManager::getEntityManager();
	shaders = ShaderManager::createShaderManager();
	std::cout << "Successfully Created Systems" << std::endl;
}

ECSEngine::~ECSEngine() {
	for(int i = 0; i < systems.size(); i++) {
		delete systems[i];
	}
}

void ECSEngine::update()
{
	for(int i = 0; i < systems.size(); i++) {
		systems[i]->update();
	}
}


int ECSEngine::addEntity()
{
	int entityID = entities->createEntity();
	Transform defaultTransform = Transform();
	defaultTransform.ownerID = entityID;
	entities->addComponent(entityID,defaultTransform);
	return entityID;
}

void ECSEngine::addSystem(System * newSystem) {
	if(newSystem == NULL) {
		std::cout << "ERROR : System being registered in ECS was NULL" << std::endl;
		exit(1);
	}
	systems.push_back(newSystem);
}
