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

using namespace std;

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
	cout << "update writing:\n" << _getRequest << "\n";
	writeMessage(_socket, _getRequest, _getRequestLength);
	HttpMessageReader(_socket, _response).readResponse();
	printf("Got: %s\n", _response.body());
	cout << "Out of update\n";
}

int Resource::createConnection(const char* address)
{
	cout << "Into createConnection:" << address << "\n";
	int sock = socket(AF_UNIX, SOCK_STREAM, 0);
	assert(sock > -1);
	struct sockaddr_un addr;
	memset(&addr, 0, sizeof(struct sockaddr_un));
	addr.sun_family = AF_UNIX;
	strncpy(addr.sun_path, address, sizeof(addr.sun_path) - 1);
	assert(connect(sock, (const sockaddr*)&addr, sizeof(struct sockaddr_un)) == 0);
	
	cout << "Outof createConnection\n";
	return sock;
}

int Resource::createSocket(const char* protocol)
{
	cout << "Into createSocket\n";	
	static const char openStreamRequestTemplate[] =  
		"GET %s HTTP/1.1\r\n"
		"Upgrade: socketstream\r\n"
		"Connection: Upgrade\r\n"
		"Host: localhost\r\n"
		"\r\n"; 

	
	int socket = createConnection(_address);
	char openStreamRequest[256];
	sprintf(openStreamRequest, openStreamRequestTemplate, _path);
	writeMessage(socket, openStreamRequest, sizeof(openStreamRequest));
	HttpMessage handshakeResponse;
	HttpMessageReader(socket, handshakeResponse).readResponse();
	if (handshakeResponse.headerValue(Header::connection) == 0 || strcmp(handshakeResponse.headerValue(Header::connection), "Upgrade") != 0) {
		return -1;
	}
	if (handshakeResponse.headerValue(Header::upgrade) == 0 || strcmp(handshakeResponse.headerValue(Header::upgrade), "socketstream") != 0) {
		return -1;
	}
	cout << "Out of createSocket";
	return socket;
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

		int errorNumber = errno;
		std::cerr << "Throwing error: " << errorNumber << ", " << strerror(errorNumber) << std::endl;
		throw errorNumber;
	}
}

