/* 
 * File:   WebSocket.h
 * Author: Christian Surlykke <christian@surlykke.dk>
 *
 * Created on 5. april 2015, 11:35
 */

#ifndef WEBSOCKET_H
#define	WEBSOCKET_H

class WebSocket
{
public:
	enum class Error : int 
	{
		BufferToSmall,
		BrokenPipe
	};
	
	WebSocket(int socket);
	virtual ~WebSocket();

	/**
	 * 
     * @return 
	 *	-1 on error
	 *  if no error: number of bytes ready to read 
     */
	int poll();
	
private:
	int open(const char* subprotocol);
	void websocketRead();	
	void websocketWrite();

	int read(char* buf, int bufsize);
	void write(char* data, int datasize);
	int _socket;
};

#endif	/* WEBSOCKET_H */

