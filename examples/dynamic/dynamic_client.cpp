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
#include "httpmessage.h"
using namespace std;
using namespace org_restfulipc;
/*
 * 
 */
#define URL "http://{org.restfulipc.examples.Dynamic}/res"
int main(int argc, char** argv)
{
    try { 
        /*int sock = connectToNotifications(URL, "");
        while (true) {
            char ch = waitForNotifications(sock);
            cout << "Got:<" << ch << ">\n";
            HttpMessage message;
            httpGet(URL, message);
        }*/
        cout << "Starting\n";
        HttpUrl url(URL);
        int sock = openConnection(url);
        for (int i = 0; i < 1000000; i++) {
            HttpMessage message;
            httpGet(sock, url, message);
        }
        close(sock);
        cout << "Ending\n";
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

