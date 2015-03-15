/* 
 * File:   httprequest.h
 * Author: Christian Surlykke <christian@surlykke.dk>
 *
 * Created on 15. marts 2015, 14:36
 */

#ifndef HTTPREQUEST_H
#define	HTTPREQUEST_H
#include "httpprotocol.h"
#include <iostream>

struct HttpRequest
{
	HttpRequest();
	virtual ~HttpRequest();

	void clear();

	Method method;
	char* path;
	char* queryString;
	char* body;

};

std::ostream& operator<<(std::ostream& out, const HttpRequest& httpRequest);

#endif	/* HTTPREQUEST_H */

