/* 
 * File:   httpprotocol.cpp
 * Author: Christian Surlykke <christian@surlykke.dk>
 * 
 * Created on 22. februar 2015, 19:55
 */

#include "httpprotocol.h"
#include <string.h>

Method fromStr(const char* str)
{

	if (strncmp("GET", str, 3) == 0) return Method::GET;
	if (strncmp("PATCH", str, 5) == 0) return Method::PATCH;
	if (strncmp("POST", str, 4) == 0) return Method::POST;
	if (strncmp("DELETE", str, 6) == 0) return Method::DELETE;
	if (strncmp("PUT", str, 3) == 0) return Method::PUT;
	if (strncmp("HEAD", str, 4) == 0) return Method::HEAD;
	if (strncmp("TRACE", str, 5) == 0) return Method::TRACE;
	if (strncmp("OPTIONS", str, 7) == 0) return Method::OPTIONS;
	if (strncmp("CONNECT", str, 7) == 0) return Method::CONNECT;

	return Method::UNKNOWN;
}

const char* header_str_value[] = 
{
	"accept-charset",
	"accept-encoding",
	"accept-language",
	"accept-datetime",
	"authorization",
	"cache-control",
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

const char* strVal(Header header)
{
	return header_str_value[(int) header];
}

const char* status_line[] =
{
	"200 OK\r\n",
	"201 Created\r\n",
	"202 Accepted\r\n",
	"203 Non-Authoritative Information \r\n",
	"204 No Content\r\n",
	"205 Reset Content\r\n",
	"206 Partial Content\r\n",
	"207 Multi-Status \r\n",
	"208 Already Reported \r\n",
	"226 IM Used \r\n",
	"400 Bad Request\r\n",
	"401 Unauthorized\r\n",
	"402 Payment Required\r\n",
	"403 Forbidden\r\n",
	"404 Not Found\r\n",
	"405 Method Not Allowed\r\n",
	"406 Not Acceptable\r\n",
	"407 Proxy Authentication Required\r\n",
	"408 Request Timeout\r\n",
	"409 Conflict\r\n",
	"410 Gone\r\n",
	"411 Length Required\r\n",
	"412 Precondition Failed\r\n",
	"413 Request Entity Too Large\r\n",
	"414 Request-URI Too Long\r\n",
	"415 Unsupported Media Type\r\n",
	"416 Requested Range Not Satisfiable\r\n",
	"417 Expectation Failed\r\n",
	"418 I'm a teapot\r\n",
	"419 Authentication Timeout\r\n",
	"422 Unprocessable Entity\r\n",
	"423 Locked\r\n",
	"424 Failed Dependency\r\n",
	"426 Upgrade Required\r\n",
	"428 Precondition Required\r\n",
	"429 Too Many Requests\r\n",
	"431 Request Header Fields Too Large\r\n",
	"440 Login Timeout\r\n",
	"444 No Response\r\n",
	"449 Retry With\r\n",
	"450 Blocked by Windows Parental Controls\r\n",
	"451 Unavailable For Legal Reasons \r\n",
	"494 Request Header Too Large \r\n",
	"496 No Cert \r\n",
	"497 HTTP to HTTPS \r\n",
	"498 Token expired/invalid \r\n",
	"499 Client Closed Request \r\n",
	"500 Internal Server Error\r\n",
	"501 Not Implemented\r\n",
	"502 Bad Gateway\r\n",
	"503 Service Unavailable\r\n",
	"504 Gateway Timeout\r\n",
	"505 HTTP Version Not Supported\r\n",
	"506 Variant Also Negotiates \r\n",
	"507 Insufficient Storage \r\n",
	"508 Loop Detected \r\n",
	"509 Bandwidth Limit Exceeded \r\n",
	"510 Not Extended \r\n",
	"511 Network Authentication Required \r\n",
	"598 Network read timeout error \r\n",
	"599 Network connect timeout error \r\n"
};


