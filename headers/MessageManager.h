#ifndef MESSAGEMANAGER_H
#define MESSAGEMANAGER_H

#include "System.h"
#include <SDL2/SDL.h>
#include <vector>

#define MESSAGE_TYPE_COUNT 6

#define KEY_EVENT 0
#define MOUSE_EVENT 1 
#define MOVEMENT_CHANGE 2
#define COLLISION 3
#define IMPULSE 4
#define ANIMATION_MESSAGE 5

class System; // forward declaration

struct BasicMessage {
	int targetEntity;
	int messageType;
	BasicMessage(int type) {
		messageType = type;
	}
	virtual void dummy(){}
	virtual ~BasicMessage(){};
};

struct KeyEvent : BasicMessage{
	SDL_Event * event;
	KeyEvent(SDL_Event * e):BasicMessage(KEY_EVENT)
	{
		event = e;
	}
};

struct MouseEvent : BasicMessage {
	SDL_Event * event;
	MouseEvent(SDL_Event * e):BasicMessage(MOUSE_EVENT)
	{
		event = e;
	}
};




class MessageManager {
private:
	std::vector<System*> subscribers[MESSAGE_TYPE_COUNT];
	static MessageManager * object;
public:
	MessageManager();
	void subscribe(System *s, int MessageType);
	void sendMessage(BasicMessage *message);
	static MessageManager * getMessageManager();
};

#endif