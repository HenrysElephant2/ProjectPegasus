#ifndef COMPONENTMANAGER_H
#define COMPONENTMANAGER_H

#include <vector>
#include "Component.h"
#include <iostream>
#include <string>

#define BASE_NUMBER_OF_COMPONENTS 3


template <class T>
class ComponentManager {
private:
	T* componentList;
	bool* valid; // use valid list to keep track of which slots in the array have been filled
	int componentListSize;
	void reallocateComponents(int numberEntities) // reallocates memory to account for more components
	{
		componentList = (T*)realloc(componentList, numberEntities * sizeof(T));
		valid = (bool*)realloc(valid, numberEntities * sizeof(bool));
		for(int i = componentListSize; i < numberEntities; i++)
		{
			valid[i] = false;
		}
		componentListSize = numberEntities;
	}



public:
	ComponentManager()
	{
		componentList = (T*)malloc(sizeof(T) * BASE_NUMBER_OF_COMPONENTS);
		componentListSize = BASE_NUMBER_OF_COMPONENTS;
		valid = (bool*)malloc(sizeof(bool) * BASE_NUMBER_OF_COMPONENTS);
		if(valid == NULL || componentList == NULL) {std::cout << "Could not allocate space for componentManager" << std::endl;}
		memset(valid,0,sizeof(bool) * BASE_NUMBER_OF_COMPONENTS);
	}
	~ComponentManager()
	{
		free(componentList);
		free(valid);
	}
	T* getComponent(int entityID)// return of null pointer means this entity doesn't have this component
	{
		if(entityID >= componentListSize)
			return 0;
		if(valid[entityID])
			return &componentList[entityID];
		else return 0;
	}	
	//T* getNext(int entityID); // maybe add this later. could be confusing since an entity might not need
	void addComponent(int entityID, T &component) // add a component, must provide entity id, and a reference to the component
	{
		if(entityID >= componentListSize)
			reallocateComponents(entityID + 1);
		memcpy((void*)(componentList + entityID), (void*)&component, sizeof(T));
		valid[entityID] = true;
	}
	void deleteComponent(int entityID) // invalidates the component for entity entityID. does not deallocate memory, the slot for this component can be filled later
	// by another component for a different, new entity that uses the same entityID
	{
		valid[entityID] = false;
	}
	void print(void (*f)(T*)) // print components, must provide a function that does the printing given a pointer to the component.
	{
		std::cout << "Printing Components:" << std::endl;
		std::cout << "list size: " << componentListSize << std::endl;
		for(int i = 0; i < componentListSize; i++)
		{
			if(valid[i])
				(*f)(componentList+i);
		}
	}
	int getSize() // returns the size of the componentList. This can be used to determine the minimum number of entities that need to be iterated through by a system
	{
		return componentListSize;
	}

};


#endif