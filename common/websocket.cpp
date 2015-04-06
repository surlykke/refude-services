/* 
 * File:   WebSocket.cpp
 * Author: Christian Surlykke <christian@surlykke.dk>
 * 
 * Created on 5. april 2015, 11:35
 */

#include <unistd.h>

#include "websocket.h"

WebSocket::WebSocket(int socket)
{
}

WebSocket::~WebSocket()
{
	close(_socket);
}

int WebSocket::poll()
{
}	

int WebSocket::read(char* buf, int bufsize)
{
	
}

void WebSocket::write(char* data, int datasize)
{
}
