/*
* Copyright (c) 2015, 2016 Christian Surlykke
*
* This file is part of the Restful Inter Process Communication (Ripc) project. 
* It is distributed under the LGPL 2.1 license.
* Please refer to the LICENSE file for a copy of the license.
*/

#ifndef HTTPPROTOCOL_H
#define    HTTPPROTOCOL_H

namespace refude
{
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

    Method string2Method(const char* str);

    const char* method2String(Method method);

    int methodLength(Method method);

    // Some well known header names 
    namespace Header
    {
        extern const char* accept_charset;
        extern const char* accept_encoding;
        extern const char* accept_language;
        extern const char* accept_datetime;
        extern const char* authorization;
        extern const char* cache_control;
        extern const char* connection;
        extern const char* cookie;
        extern const char* content_length;
        extern const char* content_md5;
        extern const char* content_type;
        extern const char* date;
        extern const char* expect;
        extern const char* from;
        extern const char* host;
        extern const char* if_match;
        extern const char* if_modified_since;
        extern const char* if_none_match;
        extern const char* if_range;
        extern const char* if_unmodified_since;
        extern const char* max_forwards;
        extern const char* origin;
        extern const char* pragma;
        extern const char* proxy_authorization;
        extern const char* range;
        extern const char* referer;
        extern const char* sec_websocket_accept;
        extern const char* sec_websocket_key;
        extern const char* sec_websocket_protocol;
        extern const char* te;
        extern const char* user_agent;
        extern const char* upgrade;
        extern const char* via;
        extern const char* warning;
        extern const char* unknown;
    }

    enum class HttpCode {
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

    const char* statusLine(HttpCode status);


}

#endif    /* HTTPPROTOCOL_H */
