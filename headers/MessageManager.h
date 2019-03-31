#ifndef MESSAGEMANAGER_H
#define MESSAGEMANAGER_H

#include "System.h"
#include <vector>

#define MESSAGE_TYPE_COUNT 5

#define KEY_EVENT 0
#define MOUSE_EVENT 1 
#define MOVEMENT_CHANGE 2
#define COLLISION 3
#define IMPULSE 4

class System; // forward declaration

struct BasicMessage {
	int targetEntity;
	int messageType;
	BasicMessage(int type) {
		messageType = type;
	}
};




class MessageManager {
private:
	std::vector<System*> subscribers[MESSAGE_TYPE_COUNT];
public:
	MessageManager();
	void subscribe(System *s, int MessageType);
	void sendMessage(BasicMessage *message);
};

#endif