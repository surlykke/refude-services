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

	int connectToNotifications(const char* url, const char* protocol);
	char waitForNotifications(int sock);
}

#endif	/* RESOURCE_H */

