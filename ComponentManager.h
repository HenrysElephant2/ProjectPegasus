#ifndef COMPONENTMANAGER_H
#define COMPONENTMANAGER_H

#include <vector>
#include "Component.h"
#include <iostream>
#include <string>

#define BASE_NUMBER_OF_ENTITIES 3


template <class T>
class ComponentManager {
private:
	T* componentList;
	bool* valid; // use valid list to keep track of which slots in the array have been filled
	int componentListSize;
	void reallocateComponents(int numberEntities); // reallocates memory to account for more components
	



public:
	ComponentManager();
	~ComponentManager();
	T* getComponent(int entityID);// return of null pointer means this entity doesn't have this component
	//T* getNext(int entityID); // maybe add this later. could be confusing since an entity might not need
	void addComponent(int entityID, T &component); // add a component, must provide entity id, and a reference to the component
	void deleteComponent(int entityID); // invalidates the component for entity entityID. does not deallocate memory, the slot for this component can be filled later
	// by another component for a different, new entity that uses the same entityID
	void print(void (*f)(T*)); // print components, must provide a function that does the printing given a pointer to the component.


};

#endif