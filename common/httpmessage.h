/* 
 * File:   httprequest.h
 * Author: Christian Surlykke <christian@surlykke.dk>
 *
 * Created on 15. marts 2015, 14:36
 */

#ifndef HTTPMESSAGE_H
#define	HTTPMESSAGE_H
#include <iostream>

#include "httpprotocol.h"

struct HttpMessageReader;

struct HttpMessage
{
	HttpMessage();
	virtual ~HttpMessage();
	void clear();

	inline Method method() const { return _method; }
	inline const char* path() const { return _path; }
	inline const char* queryString() const { return _queryString; }
	inline int status() const { return _status; } 
	inline const char* headerValue(Header h) const { return _headers[(int) h]; }
	inline const char* body() const { return _body; }
	inline int contentLength() const { return _contentLength; }

private:
	friend HttpMessageReader;	
	
	Method _method;
	char* _path;
	char* _queryString;
	int _status;
	const char* _headers[(int) Header::unknown];
	char* _body;
	int _contentLength;
	
	char _buffer[8192];
};

struct HttpMessageReader
{
	enum Error
	{
		InvalidFormat,
		MissingContentLength,
		ConnectionBroken
	};

	HttpMessageReader(int socket, HttpMessage& message);
	void readRequest();
	void readResponse();
	void readRequestLine();
	void readStatusLine();
	void readHeaderLines();
	bool readHeaderLine();
	void readHeaders();
	void readBody();
	char currentChar();
	char nextChar();
	bool isTChar(char c);
	bool isFChar(char c);
	void receive();

	void clear();
	void assert(bool condition, Error error = Error::InvalidFormat);


	int _socket;
	HttpMessage& _message;
	int _bufferEnd;
	int _currentPos;
};


//std::ostream& operator<<(std::ostream& out, const HttpMessage& httpRequest);

#endif	/* HTTPMESSAGE_H */

