/* 
 * File:   websocket.h
 * Author: Christian Surlykke <christian@surlykke.dk>
 *
 * Created on 30. marts 2015, 16:32
 */

#ifndef WEBSOCKET_H
#define	WEBSOCKET_H

class WebSocket
{
public:
	WebSocket();
	virtual ~WebSocket();
private:
	static const char* handshakeRequestTemplate;
};

#endif	/* WEBSOCKET_H */

