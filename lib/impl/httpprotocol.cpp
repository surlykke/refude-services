/*
* Copyright (c) 2015, 2016 Christian Surlykke
*
* This file is part of the Restful Inter Process Communication (Ripc) project. 
* It is distributed under the LGPL 2.1 license.
* Please refer to the LICENSE file for a copy of the license.
*/


#include "httpprotocol.h"
#include <string.h>
namespace refude
{
    Method string2Method(const char* str)
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
    
    const char* method2String(Method method) {
        static const char* stringVals[] = {
            "GET",
            "PATCH",
            "POST",
            "DELETE",
            "PUT",
            "HEAD",
            "TRACE",
            "OPTIONS",
            "CONNECT",
        };
        return (method <= Method::CONNECT ? stringVals[(int) method] : "UNKNOWN");
    }

    int methodLengths[] = { 3, 5, 4, 6, 3, 4, 5, 7, 7 };

    int methodLength(Method method)
    {
        return methodLengths[(int) method];
    }

    namespace Header
    {
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
        const char* sec_websocket_accept = "sec-websocket-accept";
        const char* sec_websocket_key = "sec-websocket-key";
        const char* sec_websocket_protocol = "sec-websocket-protocol";
        const char* te = "te";
        const char* user_agent = "user-agent";
        const char* upgrade = "upgrade";
        const char* via = "via";
        const char* warning = "warning";
        const char* unknown = "unknown";
    };


    const char* status_line[] =
    {
        "HTTP/1.1 200 OK\r\n",
        "HTTP/1.1 201 Created\r\n",
        "HTTP/1.1 202 Accepted\r\n",
        "HTTP/1.1 203 Non-Authoritative Information \r\n",
        "HTTP/1.1 204 No Content\r\n",
        "HTTP/1.1 205 Reset Content\r\n",
        "HTTP/1.1 206 Partial Content\r\n",
        "HTTP/1.1 207 Multi-Status \r\n",
        "HTTP/1.1 208 Already Reported \r\n",
        "HTTP/1.1 226 IM Used \r\n",
        "HTTP/1.1 400 Bad Request\r\n",
        "HTTP/1.1 401 Unauthorized\r\n",
        "HTTP/1.1 402 Payment Required\r\n",
        "HTTP/1.1 403 Forbidden\r\n",
        "HTTP/1.1 404 Not Found\r\n",
        "HTTP/1.1 405 Method Not Allowed\r\n",
        "HTTP/1.1 406 Not Acceptable\r\n",
        "HTTP/1.1 407 Proxy Authentication Required\r\n",
        "HTTP/1.1 408 Request Timeout\r\n",
        "HTTP/1.1 409 Conflict\r\n",
        "HTTP/1.1 410 Gone\r\n",
        "HTTP/1.1 411 Length Required\r\n",
        "HTTP/1.1 412 Precondition Failed\r\n",
        "HTTP/1.1 413 Request Entity Too Large\r\n",
        "HTTP/1.1 414 Request-URI Too Long\r\n",
        "HTTP/1.1 415 Unsupported Media Type\r\n",
        "HTTP/1.1 416 Requested Range Not Satisfiable\r\n",
        "HTTP/1.1 417 Expectation Failed\r\n",
        "HTTP/1.1 418 I'm a teapot\r\n",
        "HTTP/1.1 419 Authentication Timeout\r\n",
        "HTTP/1.1 422 Unprocessable Entity\r\n",
        "HTTP/1.1 423 Locked\r\n",
        "HTTP/1.1 424 Failed Dependency\r\n",
        "HTTP/1.1 426 Upgrade Required\r\n",
        "HTTP/1.1 428 Precondition Required\r\n",
        "HTTP/1.1 429 Too Many Requests\r\n",
        "HTTP/1.1 431 Request Header Fields Too Large\r\n",
        "HTTP/1.1 440 Login Timeout\r\n",
        "HTTP/1.1 444 No Response\r\n",
        "HTTP/1.1 449 Retry With\r\n",
        "HTTP/1.1 450 Blocked by Windows Parental Controls\r\n",
        "HTTP/1.1 451 Unavailable For Legal Reasons \r\n",
        "HTTP/1.1 494 Request Header Too Large \r\n",
        "HTTP/1.1 496 No Cert \r\n",
        "HTTP/1.1 497 HTTP to HTTPS \r\n",
        "HTTP/1.1 498 Token expired/invalid \r\n",
        "HTTP/1.1 499 Client Closed Request \r\n",
        "HTTP/1.1 500 Internal Server Error\r\n",
        "HTTP/1.1 501 Not Implemented\r\n",
        "HTTP/1.1 502 Bad Gateway\r\n",
        "HTTP/1.1 503 Service Unavailable\r\n",
        "HTTP/1.1 504 Gateway Timeout\r\n",
        "HTTP/1.1 505 HTTP Version Not Supported\r\n",
        "HTTP/1.1 506 Variant Also Negotiates \r\n",
        "HTTP/1.1 507 Insufficient Storage \r\n",
        "HTTP/1.1 508 Loop Detected \r\n",
        "HTTP/1.1 509 Bandwidth Limit Exceeded \r\n",
        "HTTP/1.1 510 Not Extended \r\n",
        "HTTP/1.1 511 Network Authentication Required \r\n",
        "HTTP/1.1 598 Network read timeout error \r\n",
        "HTTP/1.1 599 Network connect timeout error \r\n"
    };


    const char* statusLine(HttpCode status) 
    {
        return status_line[(int)status];
    }
}
