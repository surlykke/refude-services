/* 
 * File:   resource.h
 * Author: Christian Surlykke <christian@surlykke.dk>
 *
 * Created on 30. marts 2015, 12:15
 */

#ifndef RESOURCE_H
#define	RESOURCE_H

#include <curl/curl.h>
#include "../common/httpmessage.h"

/*class Resource
{
public:
	Resource(const char* address, const char *path);
	virtual ~Resource();

	void update();	
	int createSocket(const char* protocol);
	

private:
	int createConnection(const char* address);
	inline void writeMessage(int socket, const char* data, int nbytes);
	inline int writeSome(int socket, const char* data, int nbytes);
	void assert(bool condition);
	
	const char* _address;
	const char* _path;

	int _socket;
	HttpMessage _response;
	char _getRequest[128];
	int _getRequestLength;
};*/

namespace org_restfulipc
{
	void httpGet(const char* url, HttpMessage& message);
	void httpGet(int socket, const char* path, HttpMessage& message);	

	//http://host:port/path
	//http://host:{udspath}/path

	int connectToSocket(const char* service, const char* path, const char* protocol);
}

#endif	/* RESOURCE_H */

