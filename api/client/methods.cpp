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

	// http[s]://host[:port][/path]
	// http[s]://{socketpath}[/path]
	struct HttpUrl
	{
		int domain; // AF_UNIX or AF_INET
		const char* hostname;
		int port;
		const char* socketPath;
		const char* requestPath;

		void readAfUnixUrl(char* pos)
		{
			domain = AF_UNIX;	
			socketPath = pos + 1;
			pos++;
			while (*pos != '}' && *pos != '\0') {
				pos++;
			}
		
			if (*pos == '\0') {
				throw "Invalid url";
			}

			*pos = '\0';
			requestPath = pos + 1;
		}

		void readAfInetUrl(char* pos)
		{
			throw "Not implemented yet";
		}

		void readUrl()
		{
			char* pos = url;
			if (strncmp("http://", url, 7)) {
				throw "Unknown protocol";
			}

			pos += 7;
				
			if (*pos = '{')	{
				readAfUnixUrl(pos);
			}
			else {
				readAfInetUrl(pos);
			}
	
		}

		HttpUrl(const char* url)
		{
			if (strlen(url) > 127) {
				throw "Url too long!";
			}

			strcpy(this->url, url);
			
			readUrl();	
		}

	private:
		char url[128];
	};




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

	void httpGet(const char* url, HttpMessage& message)
	{
		static const char* getRequestTemplate = 
			"GET %s HTTP/1.1\r\n"
			"Host: localhost\r\n"
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

	int connectToNotifications(const char* url, const char* protocol)
	{
		HttpUrl httpUrl(url);
		int sock = openConnection(httpUrl);

		static const char openStreamRequestTemplate[] =  
			"GET %s HTTP/1.1\r\n"
			"Upgrade: socketstream\r\n"
			"Connection: Upgrade\r\n"
			"Host: localhost\r\n"
			"\r\n"; 

		char openStreamRequest[256];
		sprintf(openStreamRequest, openStreamRequestTemplate, httpUrl.requestPath);
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
