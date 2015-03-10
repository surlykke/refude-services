/* 
 * File:   HttpRequest.h
 * Author: Christian Surlykke <christian@surlykke.dk>
 *
 * Created on 20. februar 2015, 22:06
 */

#ifndef HTTPREQUEST_H
#define	HTTPREQUEST_H


#include "httpprotocol.h"

struct HttpRequest {
	HttpRequest();
	HttpRequest(const HttpRequest& orig);
	virtual ~HttpRequest();
	
	HttpOperation::Verb operation;
	const char* path;

	const char*** headers;
	const char*** queryParameters;


};


class AbstractResource
{
protected:
	AbstractResource();
	bool canGET;
	bool canPATCH;
	bool canPOST;
	bool canPUT;
	bool canOPTIONS;
};

#endif	/* HTTPREQUEST_H */

