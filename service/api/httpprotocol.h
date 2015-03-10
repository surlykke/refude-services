/* 
 * File:   httpprotocol.h
 * Author: Christian Surlykke <christian@surlykke.dk>
 *
 * Created on 22. februar 2015, 19:55
 */

#ifndef HTTPPROTOCOL_H
#define	HTTPPROTOCOL_H


// HTTP Methods

struct HttpOperation {

	enum class Verb {
		GET,
		PATCH,
		POST,
		DELETE,
		PUT,
		HEAD,
		TRACE,
		OPTIONS,
		CONNECT
	};

};
// Headers that we care about
/*const char* accept = "accept";
const char* accept_charset = "accept-charset";
const char* accept_encoding = "accept-encoding";
const char* accept_language = "accept-language";
const char* accept_datetime = "accept-datetime";
const char* authorization = "authorization";
const char* cache_control = "cache-control";
const char* connection = "connection";
const char* cookie = "cookie";
const char* content_length = "content-length";
const char* content_md5 = "content-md5";
const char* content_type = "content-type";
const char* date = "date";
const char* expect = "expect";
const char* from = "from";
const char* host = "host";
const char* if_match = "if-match";
const char* if_modified_since = "if-modified-since";
const char* if_none_match = "if-none-match";
const char* if_range = "if-range";
const char* if_unmodified_since = "if-unmodified-since";
const char* max_forwards = "max-forwards";
const char* origin = "origin";
const char* pragma = "pragma";
const char* proxy_authorization = "proxy-authorization";
const char* range = "range";
const char* referer = "referer";
const char* te = "te";
const char* user_agent = "user-agent";
const char* upgrade = "upgrade";
const char* via = "via";
const char* warning = "warning";

// Canned answers
const char* Http200 =
	"200 OK\r\n"
	"\r\n";

const char* Http201 =
	"201 Created\r\n"
	"\r\n";

const char* Http202 =
	"202 Accepted\r\n"
	"\r\n";

const char* Http203 =
	"203 Non-Authoritative Information \r\n"
	"\r\n";

const char* Http204 =
	"204 No Content\r\n"
	"\r\n";

const char* Http205 =
	"205 Reset Content\r\n"
	"\r\n";

const char* Http206 =
	"206 Partial Content\r\n"
	"\r\n";

const char* Http207 =
	"207 Multi-Status \r\n"
	"\r\n";

const char* Http208 =
	"208 Already Reported \r\n"
	"\r\n";

const char* Http226 =
	"226 IM Used \r\n"
	"\r\n";

const char* Http400 =
	"400 Bad Request\r\n"
	"\r\n";

const char* Http401 =
	"401 Unauthorized\r\n"
	"\r\n";

const char* Http402 =
	"402 Payment Required\r\n"
	"\r\n";

const char* Http403 =
	"403 Forbidden\r\n"
	"\r\n";

const char* Http404 =
	"404 Not Found\r\n"
	"\r\n";

const char* Http405 =
	"405 Method Not Allowed\r\n"
	"\r\n";

const char* Http406 =
	"406 Not Acceptable\r\n"
	"\r\n";

const char* Http407 =
	"407 Proxy Authentication Required\r\n"
	"\r\n";

const char* Http408 =
	"408 Request Timeout\r\n"
	"\r\n";

const char* Http409 =
	"409 Conflict\r\n"
	"\r\n";

const char* Http410 =
	"410 Gone\r\n"
	"\r\n";

const char* Http411 =
	"411 Length Required\r\n"
	"\r\n";

const char* Http412 =
	"412 Precondition Failed\r\n"
	"\r\n";

const char* Http413 =
	"413 Request Entity Too Large\r\n"
	"\r\n";

const char* Http414 =
	"414 Request-URI Too Long\r\n"
	"\r\n";

const char* Http415 =
	"415 Unsupported Media Type\r\n"
	"\r\n";

const char* Http416 =
	"416 Requested Range Not Satisfiable\r\n"
	"\r\n";

const char* Http417 =
	"417 Expectation Failed\r\n"
	"\r\n";

const char* Http418 =
	"418 I'm a teapot\r\n"
	"\r\n";

const char* Http419 =
	"419 Authentication Timeout\r\n"
	"\r\n";

const char* Http422 =
	"422 Unprocessable Entity\r\n"
	"\r\n";

const char* Http423 =
	"423 Locked\r\n"
	"\r\n";

const char* Http424 =
	"424 Failed Dependency\r\n"
	"\r\n";

const char* Http426 =
	"426 Upgrade Required\r\n"
	"\r\n";

const char* Http428 =
	"428 Precondition Required\r\n"
	"\r\n";

const char* Http429 =
	"429 Too Many Requests\r\n"
	"\r\n";

const char* Http431 =
	"431 Request Header Fields Too Large\r\n"
	"\r\n";

const char* Http440 =
	"440 Login Timeout\r\n"
	"\r\n";

const char* Http444 =
	"444 No Response\r\n"
	"\r\n";

const char* Http449 =
	"449 Retry With\r\n"
	"\r\n";

const char* Http450 =
	"450 Blocked by Windows Parental Controls\r\n"
	"\r\n";

const char* Http451 =
	"451 Unavailable For Legal Reasons \r\n"
	"\r\n";

const char* Http494 =
	"494 Request Header Too Large \r\n"
	"\r\n";

const char* Http496 =
	"496 No Cert \r\n"
	"\r\n";

const char* Http497 =
	"497 HTTP to HTTPS \r\n"
	"\r\n";

const char* Http498 =
	"498 Token expired/invalid \r\n"
	"\r\n";

const char* Http499 =
	"499 Client Closed Request \r\n"
	"\r\n";

const char* Http500 =
	"500 Internal Server Error\r\n"
	"\r\n";

const char* Http501 =
	"501 Not Implemented\r\n"
	"\r\n";

const char* Http502 =
	"502 Bad Gateway\r\n"
	"\r\n";

const char* Http503 =
	"503 Service Unavailable\r\n"
	"\r\n";

const char* Http504 =
	"504 Gateway Timeout\r\n"
	"\r\n";

const char* Http505 =
	"505 HTTP Version Not Supported\r\n"
	"\r\n";

const char* Http506 =
	"506 Variant Also Negotiates \r\n"
	"\r\n";

const char* Http507 =
	"507 Insufficient Storage \r\n"
	"\r\n";

const char* Http508 =
	"508 Loop Detected \r\n"
	"\r\n";

const char* Http509 =
	"509 Bandwidth Limit Exceeded \r\n"
	"\r\n";

const char* Http510 =
	"510 Not Extended \r\n"
	"\r\n";

const char* Http511 =
	"511 Network Authentication Required \r\n"
	"\r\n";

const char* Http598 =
	"598 Network read timeout error \r\n"
	"\r\n";

const char* Http599 =
	"599 Network connect timeout error \r\n"
	"\r\n";
*/

#endif	/* HTTPPROTOCOL_H */

