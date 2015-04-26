/* 
 * File:   notifierwebsocket.h
 * Author: Christian Surlykke <christian@surlykke.dk>
 *
 * Created on 21. april 2015, 22:52
 */

#ifndef NOTIFIER_H
#define	NOTIFIER_H

#include <poll.h>

/**
 * Sits on the serverside of a websocket notifying the client when the associated 
 * resource has changed. Handles multiple clients (each on one connection).
 * 
 */
class Notifier
{
public:
	Notifier(int resourceEventsFd, int webSocketsFd);
	virtual ~Notifier();
	void run();
	
protected:
	void handleIncomingPing(int socket);
	void sendClose(int socket);

private:
	void readResourceEvent();
	void readNewSockets();
	void removeSocket(int index);
	void dataFromClient(int fd);
	int readInt(int fd);
	struct pollfd *fds;
	nfds_t nfds;
	
};

#endif	/* NOTIFIER_H */

