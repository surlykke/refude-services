/* 
 * File:   resource.cpp
 * Author: Christian Surlykke <christian@surlykke.dk>
 * 
 * Created on 30. marts 2015, 12:15
 */

#include <string.h>
#include <sys/un.h>
#include <unistd.h>
#include <errno.h>

#include "resource.h"

Resource::Resource(const char* address, const char* path)
{
	const char* getRequestTemplate = 
		"GET %s HTTP/1.1\r\n"
		"Host: localhost\r\n"
		"\r\n";
	
	sprintf(_getRequest, getRequestTemplate, path);
	_getRequestLength = strlen(_getRequest);
}


Resource::~Resource()
{
}

void Resource::update()
{
	int socket = createConnection("org.restfulipc.examples.Power", "/Version");

	int bytesWritten = 0;
	do 
	{
		bytesWritten += write(socket, _getRequest, _getRequestLength - bytesWritten);
	}
	while (bytesWritten < _getRequestLength);

	HttpMessageReader(socket, _response).readMessage(false);
	printf("Got: %s\n", _response.body());
}

int Resource::createConnection(const char* port, const char* path)
{
	int sock = socket(AF_UNIX, SOCK_STREAM, 0);
	assert(sock > -1);
	struct sockaddr_un addr;
	memset(&addr, 0, sizeof(struct sockaddr_un));
	addr.sun_family = AF_UNIX;
	strncpy(addr.sun_path, port, sizeof(addr.sun_path) - 1);
	assert(connect(sock, (const sockaddr*)&addr, sizeof(struct sockaddr_un)) == 0);

	return sock;
}

void Resource::assert(bool condition)
{
	if (! condition)
	{
		throw errno;
	}
}


/*int Resource::openWebsocket(const char* path, const char* subprotocol)
{
	static const char* openHandshakeRequestTemplate = 
		"GET %s HTTP/1.1\r\n"
		"Upgrade: websocket\r\n"
		"Connection: Upgrade\r\n"
		"Sec-WebSocket-Protocol: org.restfulipc.notification\r\n"
		"Sec-WebSocket-Version: 13\r\n"
		"Host: localhost\r\n"; 

	int websocket = socket(AF_UNIX, SOCK_STREAM, 0);
	assert(websocket >= 0);
	struct sockaddr_un addr;
	memset(&addr, 0, sizeof(struct sockaddr_un));
	addr.sun_family = AF_UNIX;
	const char* sockName = "org.restfulipc.examples.Power";
	strncpy(addr.sun_path, sockName, sizeof(addr.sun_path) - 1);
	assert(connect(websocket, (const sockaddr*)&addr, sizeof(struct sockaddr_un)) == 0);

	char openHandshakeRequest[strlen(openHandshakeRequestTemplate) + 10];
	sprintf(openHandshakeRequest, openHandshakeRequestTemplate, "/Bat0");
	assert(write(websocket, openHandshakeRequest, strlen(openHandshakeRequest)) > -1);

	char response[1024];
	int nread = read(websocket, response, 1024);
	assert(nread > -1);
	response[nread] = '\0';
	// FIXME check response

	return websocket;
}

void Resource::assert(bool condition)
{
	if (!condition) throw errno;
}*/
