/* 
 * File:   main.cpp
 * Author: Christian Surlykke <christian@surlykke.dk>
 *
 * Created on 30. marts 2015, 11:27
 */

#include <iostream>
#include <unistd.h>
#include <string.h>

#include "methods.h"

using namespace std;
using namespace org_restfulipc;
/*
 * 
 */
int main(int argc, char** argv)
{
	try { 
		while (true) {
			HttpMessage message;
			httpGet("http://{org.restfulipc.examples.Dynamic}/res", message);
			cout << "Got:<" << message.body() << ">\n";
			sleep(5);
		}
	}
	catch (const char* errMsg) {
		cout << "Error: " << errMsg;
	}
	catch (int errornumber) {
		cout << "Errno: " << errornumber << " - " << strerror(errornumber) << "\n";
	}
}

