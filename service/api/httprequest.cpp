/* 
 * File:   httprequest.cpp
 * Author: Christian Surlykke <christian@surlykke.dk>
 * 
 * Created on 15. marts 2015, 14:36
 */

#include <iosfwd>

#include "httprequest.h"

HttpRequest::HttpRequest()
{
	clear();
}

HttpRequest::~HttpRequest()
{
}

void HttpRequest::clear()
{
	method = Method::UNKNOWN;
	path = 0; 
	queryString = 0;
	body = 0;	
}

std::ostream& operator <<(std::ostream& out, const HttpRequest& httpRequest)
{
	out << "path:" << httpRequest.path << std::endl
	    << "queryString:" << httpRequest.queryString << std::endl
	    << "body:" << httpRequest.body << std::endl;

	return out;
}