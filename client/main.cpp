/* 
 * File:   main.cpp
 * Author: Christian Surlykke <christian@surlykke.dk>
 *
 * Created on 30. marts 2015, 11:27
 */

#include <iostream>
#include <curl/curl.h>

#include "resource.h"
using namespace std;

/*
 * 
 */
int main(int argc, char** argv)
{
	cout << "Hello world!" << std::endl;

	Resource resource("org.restfulipc.examples.Power");	
	
	resource.update();

    return 0;
}

