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

#include "errorhandling.h"
#include "methods.h"

using namespace std;

namespace org_restfulipc
{



	HttpUrl::HttpUrl(const char* url)
	{
		if (strlen(url) > 127) {
			throw "Url too long!";
		}

		strcpy(this->url, url);
		
		char* pos = this->url;
		if (strncmp("http://", url, 7)) {
			throw "Unknown protocol";
		}

		pos += 7;
			
		if (*pos = '{')	{
			domain = AF_UNIX;	
			socketPath = pos + 1;
			pos = index(pos, '}');	
		
			if (!pos) {
				throw "Invalid url";
			}

			*pos = '\0';
			requestPath = pos + 1;		
		}
		else {
			throw "Not implemented yet";
		}
	}

	int writeSome(int socket, const char* data, int nbytes)
	{
		int written = send(socket, data, nbytes, MSG_NOSIGNAL);
		assert(written > -1);
		return written;
	}


	void writeMessage(int socket, const char* data, int nbytes)
	{
		for (int i = 0; i < nbytes; i += writeSome(socket, data + i, nbytes - i));
	}

	int openConnection(const HttpUrl& url)
	{
		if (url.domain == AF_UNIX) {
			int sock = socket(AF_UNIX, SOCK_STREAM, 0);
			assert(sock > -1);
			struct sockaddr_un addr;
			memset(&addr, 0, sizeof(struct sockaddr_un));
			addr.sun_family = AF_UNIX;
			strncpy(addr.sun_path, url.socketPath, sizeof(addr.sun_path) - 1);
			assert(connect(sock, (const sockaddr*)&addr, sizeof(struct sockaddr_un)) == 0);
			return sock;
		}
		else {
			throw "Not implemented";
		}
	}

	/**
	 * For one-off requests. Tells the server to close the connection when the request is served.
     * @param url
     * @param message
     */	
	void httpGet(const char* url, HttpMessage& message)
	{
		static const char* getRequestTemplate = 
			"GET %s HTTP/1.1\r\n"
			"Host: localhost\r\n"
			"Connection: close\r\n"
			"\r\n";
	
		HttpUrl httpUrl(url);
		int sock = openConnection(httpUrl);
		char request[128];		
		sprintf(request, getRequestTemplate, httpUrl.requestPath);
		writeMessage(sock, request, strlen(request));

		HttpMessageReader httpMessageReader(sock, message);
		httpMessageReader.readResponse();
		close(sock);
	}

	/**
     * @param socket
     * @param path
     * @param message
     */
	void httpGet(int sock, const HttpUrl& url, HttpMessage& message)
	{
		static const char* getRequestTemplate = 
			"GET %s HTTP/1.1\r\n"
			"Host: localhost\r\n"
			"\r\n";
	
		char request[128];		
		sprintf(request, getRequestTemplate, url.requestPath);
		writeMessage(sock, request, strlen(request));

		HttpMessageReader httpMessageReader(sock, message);
		httpMessageReader.readResponse();
	}

	int connectToNotifications(const HttpUrl& url, const char* protocol)
	{
		int sock = openConnection(url);

		static const char openStreamRequestTemplate[] =  
			"GET %s HTTP/1.1\r\n"
			"Upgrade: socketstream\r\n"
			"Connection: Upgrade\r\n"
			"Host: localhost\r\n"
			"\r\n"; 

		char openStreamRequest[256];
		sprintf(openStreamRequest, openStreamRequestTemplate, url.requestPath);
		writeMessage(sock, openStreamRequest, strlen(openStreamRequest));

		HttpMessage handshakeResponse;
		HttpMessageReader(sock, handshakeResponse).readResponse();
		
		if (handshakeResponse.headerValue(Header::connection) == 0 || 
			strcmp(handshakeResponse.headerValue(Header::connection), "Upgrade") != 0) {
			return -1;
		}
		
		if (handshakeResponse.headerValue(Header::upgrade) == 0 || 
			strcmp(handshakeResponse.headerValue(Header::upgrade), "socketstream") != 0) /*FIXME*/{
			return -1;
		}
		
		return sock;
	}

	char waitForNotifications(int sock)
	{
		char ch; 
		
		assert(read(sock, &ch, 1) > 0);

		return ch;
	}
}
