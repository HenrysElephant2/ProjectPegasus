#ifndef ENTITYMANAGER_H
#define ENTITYMANAGER_H

#include <iostream>
#include <cstring>
#include <map>


struct Entity {
	bool exists; // could add type info here
};

#define BASE_NUMBER_OF_ENTITIES 1

class EntityManager {
private:
	Entity* entities; // might need to change this later
	std::map<std::string, int> namedEntities;
	int arraySize;
	int minAvailable;
	void increaseArraySize(int size);

public:
	EntityManager();
	~EntityManager();
	int createEntity(); //returns entityID for the created entity
	int createEntity(char * name);
	void killEntity(int entityID);
	bool isAlive(int entityID);
	void print();
	int getEntityID(std::string & name);

};

#endif