/* 
 * File:   http_headers.h
 * Author: Christian Surlykke <christian@surlykke.dk>
 *
 * Created on 17. februar 2015, 07:38
 */

#ifndef HTTPHEADERS_H
#define	HTTPHEADERS_H

class HttpHeaders {
public:
	static const char* headers[];
    HttpHeaders();
    HttpHeaders(const HttpHeaders& orig);
    virtual ~HttpHeaders();
private:

};

#endif	/* HTTPHEADERS_H */

