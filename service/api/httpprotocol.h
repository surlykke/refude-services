/* 
 * File:   httpprotocol.h
 * Author: Christian Surlykke <christian@surlykke.dk>
 *
 * Created on 22. februar 2015, 19:55
 */

#ifndef HTTPPROTOCOL_H
#define	HTTPPROTOCOL_H

enum class Method {
	GET,
	PATCH,
	POST,
	DELETE,
	PUT,
	HEAD,
	TRACE,
	OPTIONS,
	CONNECT,
	UNKNOWN
};

Method fromStr(const char* str);

enum class Header {
	accept_charset,
	accept_encoding,
	accept_language,
	accept_datetime,
	authorization,
	cache_control,
	connection,
	cookie,
	content_length,
	content_md5,
	content_type,
	date,
	expect,
	from,
	host,
	if_match,
	if_modified_since,
	if_none_match,
	if_range,
	if_unmodified_since,
	max_forwards,
	origin,
	pragma,
	proxy_authorization,
	range,
	referer,
	te,
	user_agent,
	upgrade,
	via,
	warning
};

const char *strVal(Header header);

enum class Status {
	Http200,
	Http201,
	Http202,
	Http203,
	Http204,
	Http205,
	Http206,
	Http207,
	Http208,
	Http226,
	Http400,
	Http401,
	Http402,
	Http403,
	Http404,
	Http405,
	Http406,
	Http407,
	Http408,
	Http409,
	Http410,
	Http411,
	Http412,
	Http413,
	Http414,
	Http415,
	Http416,
	Http417,
	Http418,
	Http419,
	Http422,
	Http423,
	Http424,
	Http426,
	Http428,
	Http429,
	Http431,
	Http440,
	Http444,
	Http449,
	Http450,
	Http451,
	Http494,
	Http496,
	Http497,
	Http498,
	Http499,
	Http500,
	Http501,
	Http502,
	Http503,
	Http504,
	Http505,
	Http506,
	Http507,
	Http508,
	Http509,
	Http510,
	Http511,
	Http598,
	Http599
};

const char* statusLine(Status status);

#endif	/* HTTPPROTOCOL_H */
