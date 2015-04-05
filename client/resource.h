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

class Resource
{
public:
	Resource(const char* address, const char *path);
	virtual ~Resource();

	void update();
	int createConnection(const char* port, const char* path);

private:
	void assert(bool condition);

	HttpMessage _response;
	char _getRequest[128];
	int _getRequestLength;
};

#endif	/* RESOURCE_H */

