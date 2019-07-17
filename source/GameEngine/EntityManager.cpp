#include "EntityManager.h"

EntityManager * EntityManager::entityManager = NULL;

EntityManager * EntityManager::getEntityManager() {
	if(entityManager == NULL)
		entityManager = new EntityManager();
	return entityManager;
}

EntityManager::EntityManager()
{
	minAvailable = 0;
}

EntityManager::~EntityManager()
{
	entityManager = NULL;
	for (std::pair<const std::string &, ComponentManagerWrapper*> element : componentManagers) {
		//std::type_info key = element.first;
		ComponentManagerWrapper* manager = element.second;
		delete manager;
	}
}


int EntityManager::createEntity() //returns entityID for the created entity
{
	if(minAvailable == entities.size()) {
		entities.push_back(true);
	}
	else {
		entities.at(minAvailable) = true;
	}
	int created = minAvailable;

	while(minAvailable < entities.size() && entities[minAvailable])
	{
		minAvailable++;
	}
	return created;
}

int EntityManager::createEntity(char * name) //returns entityID for the created entity
{
	int entityID = createEntity();
	std::string nameString(name);
	namedEntities.insert(std::pair<std::string,int>(nameString,entityID));
	return entityID;
}

bool EntityManager::addName(char * name, int entityID) {
	if(isAlive(entityID)) {
		std::string nameString(name);
		namedEntities.insert(std::pair<std::string,int>(nameString,entityID));
		return true;
	}
	return false;
}

void EntityManager::killEntity(int entityID)
{
	if(entityID < entities.size() && entityID >= 0)
	{
		entities[entityID] = false;
		if(entityID < minAvailable)
			minAvailable = entityID;
	}

	for (std::pair<std::string, int> element : namedEntities) {
		std::string name = element.first;
		int elementId = element.second;
		if(elementId == entityID) {
			namedEntities.erase(name);
		}
	}
	for (std::pair<const std::string &, ComponentManagerWrapper*> element : componentManagers) {
		element.second->deleteComponent(entityID);
	}
}

int EntityManager::getEntityID(std::string & name) {
	return namedEntities.find(name)->second;
}

bool EntityManager::isAlive(int entityID)
{
	if(entityID >= entities.size() || entityID < 0)
		return false;
	else return entities[entityID];
}

void EntityManager::print()
{
	std::cout << "minAvailable: " << minAvailable << ", arraySize: " << std::endl;
	for(int i = 0; i < entities.size(); i++)
		std::cout << "Entity " << i << ": exists: " << entities[i] << std::endl;
}






//__________________________________________debug code______________________________________
//leaving in until this code has been tested in the ECS engine as a whole
// int main()
// {
// 	EntityManager em = EntityManager();
// 	em.createEntity();
// 	em.createEntity();
// 	em.createEntity();
// 	em.print();
// 	em.killEntity(1);
// 	em.print();
// 	em.createEntity();
// 	em.print();
// 	em.createEntity();
// 	em.print();
 
// }