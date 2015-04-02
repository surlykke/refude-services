/* 
 * File:   websocket.cpp
 * Author: Christian Surlykke <christian@surlykke.dk>
 * 
 * Created on 30. marts 2015, 16:32
 */

#include "websocket.h"

const char* WebSocket::handshakeRequestTemplate = 
	"GET %s HTTP/1.1\r\n"
	"Upgrade: websocket\r\n"
	"Connection: Upgrade\r\n"
	"Sec-WebSocket-Protocol: chat, superchat\r\n"
	"Sec-WebSocket-Version: 13\r\n"
	"Sec-WebSocket-Key: %d\r\n"
	"Host: localhost\r\n";


WebSocket::WebSocket()
{
}


WebSocket::~WebSocket()
{
}

