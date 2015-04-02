/* 
 * File:   resource.h
 * Author: Christian Surlykke <christian@surlykke.dk>
 *
 * Created on 30. marts 2015, 12:15
 */

#ifndef RESOURCE_H
#define	RESOURCE_H

#include <curl/curl.h>

class Resource
{
public:
	Resource(const char *path);
	virtual ~Resource();

	void update();
private:
	
	static size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata);

	size_t curlWriteback(char *ptr, size_t size, size_t nmemb);

	char buf[8192];
	int bufpos;

	CURL *curlHandle;
};

#endif	/* RESOURCE_H */

