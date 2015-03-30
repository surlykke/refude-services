/* 
 * File:   resource.cpp
 * Author: Christian Surlykke <christian@surlykke.dk>
 * 
 * Created on 30. marts 2015, 12:15
 */

#include <string.h>

#include "resource.h"

Resource::Resource(const char* path)
{
	curlHandle = curl_easy_init();
	curl_easy_setopt(curlHandle, CURLOPT_UNIX_SOCKET_PATH, "org.restfulipc.examples.Power");
	curl_easy_setopt(curlHandle, CURLOPT_URL,"http:/Version");
	curl_easy_setopt(curlHandle, CURLOPT_WRITEFUNCTION, Resource::write_callback);
	curl_easy_setopt(curlHandle, CURLOPT_WRITEDATA, this);
	// FIXME handle errors
}


Resource::~Resource()
{
}

void Resource::update()
{
	bufpos = 0;
	curl_easy_perform(curlHandle);	
	buf[bufpos++] = '\0';
	printf("update got\n%s\n", buf);
}

size_t Resource::write_callback(char* ptr, size_t size, size_t nmemb, void* userdata)
{
	Resource *res = (Resource*) userdata;
	res->curlWriteback(ptr, size, nmemb);
}

size_t Resource::curlWriteback(char* ptr, size_t size, size_t nmemb)
{
	int nbytes = size*nmemb;
	if (bufpos + nbytes > 8192 - 1)
	{
		return 0; // FIXME - realloc
	}

	strncpy(buf + bufpos, ptr, nbytes);
	bufpos += nbytes;
	return nbytes;
}

