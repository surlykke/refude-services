/* 
 * File:   httprequest.cpp
 * Author: Christian Surlykke <christian@surlykke.dk>
 * 
 * Created on 15. marts 2015, 14:36
 */

#include <string.h>
#include <unistd.h>
#include <iosfwd>

#include "httpmessage.h"
#include "errorhandling.h"

using namespace std;

namespace org_restfulipc
{
	HttpMessage::HttpMessage()
	{
	}


	HttpMessage::~HttpMessage()
	{
	}

	void HttpMessage::clear()
	{
		_method = Method::UNKNOWN;
		_path = 0;
		_queryString = 0;
		for (int i = 0; i < (int) Header::unknown; i++)
			_headers[i] = 0;
		_body = 0;
		_contentLength = 0;
	}


	/*void HttpMessage::handleUpgrade(AbstractResource* resource)
	{
		// We only handle websocket upgrades
		assert(strcasecmp(_request.headerValue(Header::upgrade), "websocket") == 0, Status::Http406);
		const char* subprotocol = _request.headerValue(Header::sec_websocket_protocol);
		assert(subprotocol != 0);
		assert(resource->canHandleWebsocket(_request.path, _request.queryString, subprotocol), Status::Http404);
		char handshakeResponse[strlen(handshakeResponseTemplate) + 20];
		sprintf(handshakeResponse, handshakeResponseTemplate, subprotocol);
		write(_requestSocket, handshakeResponse, strlen(handshakeResponse));
		resource->addWebsocket(_requestSocket, _request.path, _request.queryString, _request.headerValue(Header::sec_websocket_protocol));
	}

	const char* HttpMessage::handshakeResponseTemplate =
		"HTTP/1.1 101 Switching Protocols"
		"Upgrade: websocket"
		"Connection: Upgrade"
		"Sec-WebSocket-Protocol: %s";
	*/



	HttpMessageReader::HttpMessageReader(int socket, HttpMessage& message) : 
		_socket(socket), 
		_message(message),
		_bufferEnd(0),
		_currentPos(-1)
	{
	}

	void HttpMessageReader::readRequest()
	{
		cout << "Into readRequest\n";
		clear();
		readRequestLine();
		cout << "request line read\n";
		//readHeaderLines();
		readHeaders();
		cout << "headers read\n";
		readBody();
		cout << "body read\n";
	}

	void HttpMessageReader::readResponse()
	{
		clear();
		cout << "clear done\n";
		readStatusLine();
		cout << "readStatusLine done\n" ;
		readHeaderLines();
		cout << "readHeaderLines done\n"; 
		readBody();
		cout << "readBody done\n";
	}



	void HttpMessageReader::readRequestLine()
	{
		int queryStringStart;

		while (nextChar() != ' ');
		
		_message._method = string2Method(_message._buffer);
		assert<Status::Http406>(_message._method != Method::UNKNOWN);
		_message._path = _message._buffer + _currentPos + 1;

		while (! isspace(nextChar()))
		{
			if (_message._buffer[_currentPos] == '?')
			{
				_message._queryString = _message._buffer + _currentPos + 1;
				_message._buffer[_currentPos] = '\0';
			}
		};

		_message._buffer[_currentPos] = '\0';

		if (_message._queryString == 0)
		{
			_message._queryString = _message._buffer + _currentPos;
		}

		int protocolStart = _currentPos + 1;

		while (! isspace(nextChar()));
		
		assert<Status::Http400>(_message._buffer[_currentPos] == '\r' && nextChar() == '\n');
	}

	void HttpMessageReader::readStatusLine()
	{
		while (!isspace(nextChar()));
		assert<Status::Http400>(_currentPos > 0);
		assert<Status::Http400>(strncmp("HTTP/1.1", _message._buffer, 8) == 0);
		while (isspace(nextChar()));
		int statuscodeStart = _currentPos;
		assert<Status::Http400>(isdigit(currentChar()));
		while (isdigit(nextChar()));
		errno = 0;
		long int status = strtol(_message._buffer + statuscodeStart, 0, 10);
		assert<Status::Http400>(status > 100 && status < 600);
		_message._status = (int) status;

		// We ignore what follows the status code. This means that a message like
		// 'HTTP/1.1 200 Completely f**cked up' will be interpreted as 
		// 'HTTP/1.1 200 Ok'
		// (Why does the http protocol specify that both the code and the text is sent?)
		while (currentChar() != '\r')
		{
			assert<Status::Http400>(currentChar() != '\n');
			nextChar();
		}

		assert<Status::Http400>(nextChar() == '\n');

		cout << "Status line:" << _message._buffer;
	}


	void HttpMessageReader::readHeaders() 
	{
		cout << "Into readHeaders\n";	
		while (true) 
		{
			if (nextChar() == '\r')	
			{
				assert<Status::Http400>(nextChar() == '\n');
				return;
			}
			
			readHeaderLine();
		}
	}

	// On entry - _currentPos points to first character of line
	bool HttpMessageReader::readHeaderLine()
	{
		int startOfHeaderLine = _currentPos;
		int startOfHeaderValue = -1;
		int endOfHeaderValue = -1;

		while (isTChar(currentChar())) nextChar();
		assert<Status::Http400>(currentChar() == ':');
		assert<Status::Http400>(_currentPos > startOfHeaderLine);
		_message._buffer[_currentPos] = '\0';

		while (isblank(nextChar()));
		endOfHeaderValue = startOfHeaderValue = _currentPos;

		while (currentChar() != '\r')
		{
			if (!isblank(currentChar()))
			{
				endOfHeaderValue = _currentPos + 1;
			}
			nextChar();
		}

		assert<Status::Http400>(nextChar() == '\n');
		_message._buffer[endOfHeaderValue] = '\0';
		Header h = string2Header(_message._buffer + startOfHeaderLine);

		if (h != Header::unknown)
		{
			_message._headers[(int) h] = _message._buffer + startOfHeaderValue;
		}
	}

	bool HttpMessageReader::isTChar(char c)
	{
		return c != ':'; // FIXME
	}


	/* TODO: Full implementation of spec
	 *  - multiline header definitions
	 *  - Illegal chars in names/values
	 *  - Normalize whitespace
	 */
	void HttpMessageReader::readHeaderLines()
	{
		for(;;)
		{
			int colonPos = -1;
			int lineStart = _currentPos + 1;

			for (;;)	
			{
				nextChar();
				assert<Status::Http400>(_message._buffer[_currentPos] != '\n');
				
				if (_message._buffer[_currentPos] == '\r')
				{
					break;
				}
				else if (_message._buffer[_currentPos] == ':')
				{
					colonPos = _currentPos;
				}
			}
			
			assert<Status::Http400>(nextChar() == '\n');
			
			if (_currentPos == lineStart + 1)
			{
				_message._body = _message._buffer + _currentPos + 1;
				return;
			}
			else 
			{	
				assert<Status::Http400>(colonPos > lineStart); // None or empty header name
				_message._buffer[colonPos] = '\0';
				_message._buffer[_currentPos - 1] = '\0';
				Header h = string2Header(_message._buffer + lineStart);

				if (h != Header::unknown)
				{
					_message._headers[(int) h] = _message._buffer + colonPos + 1;
				}
			}
		}
	}


	void HttpMessageReader::readBody()
	{
		cout << "_message._method: " << (int)_message._method << "\n";
		if (_message._method == Method::GET || _message._method == Method::HEAD)	// Others?
		{
			return;
		}
		else 
		{
			assert<Status::Http411>(_message.headerValue(Header::content_length) != 0);

			errno = 0;
			_message._contentLength = strtoul(_message.headerValue(Header::content_length), 0, 10);
			assert(errno == 0);
		
			int bodyStart = _currentPos + 1;	
			cout << "body: " << _message._buffer + bodyStart << "\n";
			while (bodyStart + _message._contentLength > _bufferEnd)	
			{
				receive();
			}
			_message._buffer[bodyStart + _message._contentLength] = '\0';
		}
	}

	char HttpMessageReader::currentChar()
	{
		return _message._buffer[_currentPos];
	}


	char HttpMessageReader::nextChar()
	{
		_currentPos++;	

		while ( _currentPos >= _bufferEnd)
		{
			receive();
		}

		
		return _message._buffer[_currentPos];
	}

	void HttpMessageReader::receive()
	{
		int bytesRead = read(_socket, _message._buffer + _bufferEnd, 8190 - _bufferEnd);
		if (bytesRead < 0)	
		{
			printf("Brokent pipe\n");
			throw Status::Http403; // FIXME Broken pipe thing	
		}
		
		_message._buffer[_bufferEnd + bytesRead] = '\0';
		cout << "Received:\n" << _message._buffer + _bufferEnd;
		

		_bufferEnd += bytesRead;
	}

	void HttpMessageReader::clear()
	{
		_message.clear();	
		
		_bufferEnd = 0;
		_currentPos = -1;	
	}



	

}

