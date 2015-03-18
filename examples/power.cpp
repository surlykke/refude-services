/* 
 * File:   rfdspower.cpp
 * Author: Christian Surlykke <christian@surlykke.dk>
 * 
 * Created on 1. marts 2015, 12:01
 */

#include <unistd.h>

#include "power.h"
#include "string.h"

const char* Power::cannedResponse = 
	"HTTP/1.1 200 OK\r\n"
	"Content-Type: application/json; charset=UTF-8\r\n"
	"Content-Length: 15\r\n"
	"\r\n"
	"{\"foo\" = \"baa\"}";

Power::Power()
{
}

Power::~Power()
{
}

void Power::doGET(int socket, const char* path, const char* queryString)
{
	write(socket, cannedResponse, strlen(cannedResponse));
}



