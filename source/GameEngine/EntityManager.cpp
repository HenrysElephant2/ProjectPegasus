#include "EntityManager.h"

EntityManager::EntityManager()
{
	entities = (Entity*)malloc(sizeof(Entity) * BASE_NUMBER_OF_ENTITIES);
	arraySize = BASE_NUMBER_OF_ENTITIES;
	minAvailable = 0;
}

EntityManager::~EntityManager()
{
	free(entities);
}

void EntityManager::increaseArraySize(int size)
{
	entities = (Entity*)realloc(entities, size * sizeof(Entity));
	memset((void*)&(entities[arraySize]), 0, (size - arraySize) * sizeof(Entity));
	arraySize = size;
}

int EntityManager::createEntity() //returns entityID for the created entity
{
	if(minAvailable == arraySize)
	{
		minAvailable = arraySize;
		increaseArraySize(arraySize + 1);
	}
	int created = minAvailable;
	entities[created].exists = true;

	while(minAvailable < arraySize && entities[minAvailable].exists)
	{
		minAvailable++;
	}

	return created;

}
void EntityManager::killEntity(int entityID)
{
	if(entityID < arraySize)
	{
		entities[entityID].exists = false;
		if(entityID < minAvailable)
			minAvailable = entityID;
	}

}
bool EntityManager::isAlive(int entityID)
{
	if(entityID >= arraySize)
		return false;
	else return entities[entityID].exists;
}

void EntityManager::print()
{
	std::cout << "minAvailable: " << minAvailable << ", arraySize: " << arraySize << std::endl;
	for(int i = 0; i < arraySize; i++)
		std::cout << "Entity " << i << ": exists: " << entities[i].exists << std::endl;
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