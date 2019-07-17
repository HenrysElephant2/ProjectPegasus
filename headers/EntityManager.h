#ifndef ENTITY_MANAGER_H
#define ENTITY_MANAGER_H

#include <iostream>
#include <cstring>
#include <map>
#include <unordered_map>
#include <vector>
#include <typeinfo>

#include "ComponentManager.h"
#include "Components/Component.h"


#define BASE_NUMBER_OF_ENTITIES 1

class EntityManager {
private:
	std::vector<bool> entities;
	std::map<std::string, int> namedEntities;
	std::unordered_map<std::string, ComponentManagerWrapper * > componentManagers;
	int minAvailable;

	EntityManager();
	static EntityManager * entityManager;

public:
	
	~EntityManager();
	static EntityManager * getEntityManager();


	int createEntity(); //returns entityID for the created entity
	int createEntity(char * name);
	bool addName(char * name, int entityID);
	void killEntity(int entityID);
	bool isAlive(int entityID);
	void print();
	int getEntityID(std::string & name);


	template <class T>
	ComponentManager<T> * getComponentManager(T & dummy) {

		std::unordered_map<std::string, ComponentManagerWrapper *>::iterator it = componentManagers.find(typeid(dummy).name());
		if(it != componentManagers.end()) {
			return (ComponentManager<T>*)(it->second);
		}
		return NULL;
	}


	template <class T>
	void addComponentManager( const std::type_info &type, ComponentManager<T> * componentManager) {
		componentManagers[type.name()] = (ComponentManagerWrapper *)componentManager;
	}


	template <class T>
	bool addComponent(int entityID, T & component) { // returns whether or not adding the component succeeded
		if(entityID > entities.size() || entityID < 0 || !entities[entityID]) {
			return false;
		}
		std::unordered_map<std::string, ComponentManagerWrapper *>::iterator it = componentManagers.find(typeid(component).name());
		if(it != componentManagers.end()) {
			ComponentManager<T> * manager = (ComponentManager<T>*)(it->second);
			manager->addComponent(entityID, component);
			return true;
		}
		return false;
	}

};

#endif