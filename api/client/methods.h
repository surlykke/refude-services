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

namespace org_restfulipc
{
	void httpGet(const char* url, HttpMessage& message);
	void httpGet(int socket, const char* path, HttpMessage& message);	

	//http://host:port/path
	//http://host:{udspath}/path

	int connectToSocket(const char* service, const char* path, const char* protocol);
}

#endif	/* RESOURCE_H */

