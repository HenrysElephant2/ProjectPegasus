#include "MessageManager.h"
#include <iostream>


MessageManager::MessageManager()
{

}

void MessageManager::subscribe(System *s, int MessageType)
{	
	if(MessageType < MESSAGE_TYPE_COUNT) {
		subscribers[MessageType].push_back(s);
	}

}

void MessageManager::sendMessage(BasicMessage *message)
{
	int type = message->messageType;
	if(type < MESSAGE_TYPE_COUNT) {
		for(int i = 0; i < subscribers[type].size(); i++) {
			subscribers[type][i]->receiveMessage(message);
		}
	}
	delete message;
}

MessageManager * MessageManager::object = NULL;

MessageManager * MessageManager::getMessageManager() {
	if(object == NULL)
		object = new MessageManager();
	return object;
}