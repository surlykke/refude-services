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

Resource::Resource(const char* address, const char* path) : _address(address), _path(path)
{
	// We will probably use the same get over and over again, so lets build it 
	// now	
	const char* getRequestTemplate = 
		"GET %s HTTP/1.1\r\n"
		"Host: localhost\r\n"
		"\r\n";
	
	sprintf(_getRequest, getRequestTemplate, _path);
	_getRequestLength = strlen(_getRequest);

	_socket = createConnection(_address);
}


Resource::~Resource()
{
}

void Resource::update()
{
	writeMessage(_socket, _getRequest, _getRequestLength);
	HttpMessageReader(_socket, _response).readResponse();
	printf("Got: %s\n", _response.body());
}

int Resource::createConnection(const char* address)
{
	int sock = socket(AF_UNIX, SOCK_STREAM, 0);
	assert(sock > -1);
	struct sockaddr_un addr;
	memset(&addr, 0, sizeof(struct sockaddr_un));
	addr.sun_family = AF_UNIX;
	strncpy(addr.sun_path, address, sizeof(addr.sun_path) - 1);
	assert(connect(sock, (const sockaddr*)&addr, sizeof(struct sockaddr_un)) == 0);

	return sock;
}

WebSocket Resource::createWebsocket(const char* protocol)
{
	static const char* openHandshakeRequestTemplate =  
		"GET %s HTTP/1.1\r\n"
		"Upgrade: websocket\r\n"
		"Connection: Upgrade\r\n"
		"Sec-WebSocket-Protocol: %s\r\n"
		"Sec-WebSocket-Version: 13\r\n"
		"Sec-WebSocket-Key: %s\r\n"
		"Host: localhost\r\n"
		"\r\n"; 

	int websocket = createConnection(_address);
	const char* clientKey = "dGhlIHNhbXBsZSBub25jZQ=="; // FIXME
	char request[strlen(openHandshakeRequestTemplate) + strlen(_path) - 1];
	sprintf(request, openHandshakeRequestTemplate, _path, protocol, clientKey);
	writeMessage(websocket, request, strlen(request));

	HttpMessage handshakeResponse;
	HttpMessageReader(websocket, handshakeResponse).readResponse();
	assert(strcmp(handshakeResponse.headerValue(Header::connection), "Upgrade") == 0);
	assert(strcmp(handshakeResponse.headerValue(Header::upgrade), "websocket") == 0);
	assert(strcmp(handshakeResponse.headerValue(Header::sec_websocket_accept), "s3pPLMBiTxaQ9kYGzzhZRbK+xOo=") == 0); //FIXME
	assert(strcmp(handshakeResponse.headerValue(Header::sec_websocket_protocol), protocol) == 0);
	return WebSocket(websocket);
}

void Resource::writeMessage(int socket, const char* data, int nbytes)
{
	for (int i = 0; i < nbytes; i += writeSome(socket, data + i, nbytes - i));
}

int Resource::writeSome(int socket, const char* data, int nbytes)
{
	int written = write(socket, data, nbytes);
	assert(written > -1);
	return written;
}


void Resource::assert(bool condition)
{
	if (! condition)
	{
		throw errno;
	}
}

