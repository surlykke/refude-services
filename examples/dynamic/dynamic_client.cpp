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
#define URL "http://{org.restfulipc.examples.Dynamic}/res"
int main(int argc, char** argv)
{
	try { 
		int sock = connectToNotifications(URL, "");
		while (true) {
			char ch = waitForNotifications(sock);
			cout << "Got:<" << ch << ">\n";
			HttpMessage message;
			httpGet(URL, message);
		}
	}
	catch (Status status) {
		cout << "Http status:" << statusLine(status) << "\n";
	}
	catch (const char* errMsg) {
		cout << "Error: " << errMsg;
	}
	catch (int errornumber) {
		cout << "Errno: " << errornumber << " - " << strerror(errornumber) << "\n";
	}
}

