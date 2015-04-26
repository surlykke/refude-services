/* 
 * File:   notifierwebsocket.cpp
 * Author: Christian Surlykke <christian@surlykke.dk>
 * 
 * Created on 21. april 2015, 22:52
 */

#include <errno.h>
#include <unistd.h>
#include <stdlib.h>

#include "notifier.h"

Notifier::Notifier(int resourceEventsFd, int webSocketsFd) : fds(NULL)
{
	fds = (struct pollfd*) realloc(fds, 2*sizeof(struct pollfd));
	fds[0].fd = resourceEventsFd;
	fds[1].fd = webSocketsFd;
	fds[0].events = fds[1].events = POLLIN;
	fds[0].revents = fds[1].revents = 0;
	nfds = 2;
}

Notifier::~Notifier()
{
	for (int i = 0; i < nfds; i++)
	{
		while (close(fds[i].fd) < 0 && errno == EINTR);
	}

	free(fds);
}

void Notifier::run()
{
	for(;;)
	{
		int retval = poll(fds, nfds, -1);
		if (retval < 0)
		{
			if (errno != EINTR)
			{
				throw errno;	
			}

			continue; 
		}
			
		if (fds[0].revents | POLLIN)
		{
			readResourceEvent();
			continue;
		}

		if (fds[1].revents | POLLIN)
		{
			readNewSockets();
			continue;
		}

		for (int i = 1; i < nfds; i++)
		{
			if (fds[i].revents | POLLIN)
			{
				dataFromClient(fds[i].fd);
			}
		}
	}
}

void Notifier::readResourceEvent()
{
	char buf[20];
	bool updateEvent = false;
	int readBytes = read(fds[0].fd, buf, 20);
	if (readBytes < 1)
	{
		if (errno != EINTR)	
		{
			throw errno;
		}
	}

	for (int i = 0; i < readBytes; i++)
	{
		updateEvent = updateEvent || buf[i] == 'u';
	}

	if (updateEvent)
	{
		int index = 2;
		while (index < nfds)
		{
			int res;
			while ( (res == write(fds[index].fd, "u", 1)) < 0 && errno == EINTR); // FIXME Handle errors
			if (res < 0) throw errno;
			index++;
		}
	}
}

void Notifier::readNewSockets()
{
	int newSocket = readInt(fds[1].fd);
	fds = (struct pollfd*) realloc(fds, (nfds + 1)*sizeof(struct pollfd));
	fds[nfds].fd = newSocket;
	fds[nfds].events = POLLIN;
	fds[nfds].revents = 0;
	nfds++;
}

void Notifier::removeSocket(int index)
{
	int res;	
	while ( res = close(fds[index].fd) < 0 && errno == EINTR);
	if (res < 0) throw errno;	
	while (++index < nfds)
	{
		fds[index - 1] = fds[index];
	}
	nfds--;
}

int Notifier::readInt(int fd)
{
	int result;
	int remainingBytes = sizeof(int);
	int readBytes;
	while (remainingBytes > 0)
	{
		while ((readBytes = read(fd, &result + sizeof(int) - remainingBytes, sizeof(int))) < 0 && errno == EINTR);
		if (readBytes < 0) throw errno;
		remainingBytes -= readBytes;
	}

	return result;
}
