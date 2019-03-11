#ifndef SYSTEM_H
#define SYSTEM_H

class System {
private:
	MessageManager* messenger;

public:
	System(MessageManager* messengerObject);
	virtual void update();
	virtual void receiveMessage(BasicMessage *message); // will need to use dynamic_cast to determine message type
};

#endif