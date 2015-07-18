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
	struct HttpUrl
	{
		/**
         * @param url Can be of form:  http[s]://host[:port][/path]  or  http[s]://{socketpath}[/path]
         */	
		HttpUrl(const char* url);
		
		int domain; // AF_UNIX or AF_INET
		const char* hostname;
		int port;
		const char* socketPath;
		const char* requestPath;
		char url[128];
	};



	void httpGet(const char* url, HttpMessage& message);
	void httpGet(int socket, const HttpUrl& url, HttpMessage& message);	
	int openConnection(const HttpUrl& url);	
	int connectToNotifications(const HttpUrl& url, const char* protocol);
	char waitForNotifications(int sock);
}

#endif	/* RESOURCE_H */

