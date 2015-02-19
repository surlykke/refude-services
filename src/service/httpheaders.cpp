/* 
 * File:   http_headers.cpp
 * Author: Christian Surlykke <christian@surlykke.dk>
 * 
 * Created on 17. februar 2015, 07:38
 */

#include "httpheaders.h"

const char* HttpHeaders::headers[] = 
	{
		"accept",
		"accept-charset",
		"accept-encoding",
		"accept-language",
		"accept-datetime",
		"authorization",
		"cache-control",
		"connection",
		"connection",
		"cookie",
		"content-length",
		"content-md5",
		"content-type",
		"date",
		"expect",
		"from",
		"host",
		"if-match",
		"if-modified-since",
		"if-none-match",
		"if-range",
		"if-unmodified-since",
		"max-forwards",
		"origin",
		"pragma",
		"proxy-authorization",
		"range",
		"referer",
		"te",
		"user-agent",
		"upgrade",
		"via",
		"warning"
	};

HttpHeaders::HttpHeaders()
{
}

HttpHeaders::HttpHeaders(const HttpHeaders& orig)
{
}

HttpHeaders::~HttpHeaders()
{
}

