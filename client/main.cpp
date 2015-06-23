/* 
 * File:   main.cpp
 * Author: Christian Surlykke <christian@surlykke.dk>
 *
 * Created on 30. marts 2015, 11:27
 */

#include <iostream>
#include <curl/curl.h>
#include <unistd.h>
#include <string.h>

#include "resource.h"
using namespace std;

/*
 * 
 */
int main(int argc, char** argv)
{
	try {
		cout << "Hello world!" << std::endl;

		Resource resource("org.restfulipc.examples.Power", "/Version");	
		cout << "Resource created" << std::endl;

		resource.update();
		int updateStream = resource.createSocket("");
		if (updateStream < 0) {
			cout << "No update\n";
			return 1;
		}
		else { 
			char ch[1];
			while (true)  {
				if (read(updateStream, ch, 1) < 0) {
					cout << strerror(errno) << std::endl;
					return 1;
				}
					
				cout << ch[0] << std::endl;
			}; 
		}

	}
	catch (int errno) {
		cout << "Into catch..."	<< std::endl;
		return 1;
	}
	catch (...) {
		cout << "Error: " <<  strerror(errno) << std::endl;
	}
    return 0;
}

