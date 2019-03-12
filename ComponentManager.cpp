#include "ComponentManager.h"

template <class T>
T* ComponentManager<T>::getComponent(int entityID)
{
	if(entityID >= componentListSize)
		return 0;
	if(valid[entityID])
		return componentList[entityID];
	else return 0;
}

template <class T>
ComponentManager<T>::ComponentManager()
{
	componentList = (T*)malloc(sizeof(T) * BASE_NUMBER_OF_COMPONENTS);
	componentListSize = BASE_NUMBER_OF_COMPONENTS;
	valid = (bool*)malloc(sizeof(bool) * BASE_NUMBER_OF_COMPONENTS);
	memset(valid,0,sizeof(bool) * BASE_NUMBER_OF_COMPONENTS);
}

template <class T>
ComponentManager<T>::~ComponentManager()
{
	free(componentList);
	free(valid);
}

template <class T>
void ComponentManager<T>::addComponent(int entityID, T &component)
{
	if(entityID >= componentListSize)
		reallocateComponents(entityID + 1);
	memcpy((void*)(componentList + entityID), (void*)&component, sizeof(T));
	valid[entityID] = true;
}

template <class T>
void ComponentManager<T>::deleteComponent(int entityID)
{
	valid[entityID] = false;
}

template <class T>
void ComponentManager<T>::reallocateComponents(int numberEntities)
{
	componentList = (T*)realloc(componentList, numberEntities * sizeof(T));
	valid = (bool*)realloc(valid, numberEntities * sizeof(bool));
	for(int i = componentListSize; i < numberEntities; i++)
	{
		valid[i] = false;
	}
	componentListSize = numberEntities;
}

template <class T>
void ComponentManager<T>::print(void (*f)(T*))
{
	std::cout << "Printing Components:" << std::endl;
	std::cout << "list size: " << componentListSize << std::endl;
	for(int i = 0; i < componentListSize; i++)
	{
		if(valid[i])
			(*f)(componentList+i);
	}
}

template <class T>
int ComponentManager<T>::getSize()
{
	return componentListSize;
}

//_______________________________________Debugging code______________________________________________
// leaving this in for now, just in case i need to debug ComponentManager later, Will remove once componentManager has been tested in the context
// of the ECS engine as a whole


// struct debug:Component {
// 	int x;
// 	char name[10];
// };
// void printComponent(debug* c)
// {
// 	std::cout << "Owner: " << c->ownerID << ", x:" << c->x << ", Name: "<< c->name << std::endl;
// }

// int main()
// {
// 	ComponentManager<debug> cm = ComponentManager<debug>();
// 	debug x;
// 	x.ownerID = 0;
// 	x.x = 69;
// 	strncpy(x.name, "Hello", sizeof(x.name) - 1);
// 	debug y;
// 	y.ownerID = 1;
// 	y.x = 420;
// 	strncpy(y.name, "BlazeIt", sizeof(y.name) - 1);
// 	cm.addComponent(0, x);
// 	cm.addComponent(1,y);
// 	y.ownerID = 11;
// 	cm.addComponent(11,y);
// 	cm.print(printComponent);

// 	cm.deleteComponent(1);
// 	cm.print(printComponent);

// 	return 0;
// }

