#ifndef COMPONENTMANAGER_H
#define COMPONENTMANAGER_H

#include <vector>
#include <iostream>
#include <string>
#include <vector>
#include <map>


class ComponentManagerWrapper {
public:
	virtual void deleteComponent(int entityID) = 0;
	virtual ~ComponentManagerWrapper(){}
};


template <class T>
class ComponentManager : ComponentManagerWrapper{
private:
	std::vector<T> componentList;
	std::map<int, int> valid;
	std::vector<int> empty; // tracks empty spots in componentList



public:
	ComponentManager() {}
	~ComponentManager() {}

	T* getComponent(int entityID)// return of null pointer means this entity doesn't have this component
	{
		if(valid.find(entityID) != valid.end())
		{
			return &componentList[valid.at(entityID)];
		}
		
		return NULL;
	}	

	void addComponent(int entityID, T &component) // add a component, must provide entity id, and a reference to the component
	{
		if(valid.find(entityID) == valid.end())
		{
			if(empty.size() == 0) {
				int i = componentList.size();
				componentList.push_back(component);
				valid.insert(std::pair<int,int>(entityID,i));
			}
			else {
				componentList[empty[0]] = component;
				empty.erase(empty.begin());
			}
		}
		else componentList[valid.at(entityID)] = component;
	}

	void deleteComponent(int entityID) // invalidates the component for entity entityID. does not deallocate memory, the slot for this component can be filled later
	// by another component for a different, new entity that uses the same entityID
	{
		int location = valid.at(entityID);
		if(valid.count(entityID) > 0) {
			valid.erase(entityID);
			empty.push_back(location);
		}

	}

	int getSize() // returns the size of the componentList. This can be used to determine the minimum number of entities that need to be iterated through by a system
	{
		if(valid.empty())
			return 0;
		return valid.rbegin()->first + 1;
	}

};


#endif