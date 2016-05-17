/*
* Copyright (c) 2015, 2016 Christian Surlykke
*
* This file is part of the Restful Inter Process Communication (Ripc) project. 
* It is distributed under the LGPL 2.1 license.
* Please refer to the LICENSE file for a copy of the license.
*/

/* 
 * File:   test_socket_speed.cpp
 * Author: christian
 *
 * Created on 18. juli 2015, 15:56
 */

#include <string.h>
#include <iostream>
#include <sys/socket.h>
#include <linux/un.h>
#include <unistd.h>
#include "errorhandling.h" 

#define SOCK_PATH "test_speed"

using namespace org_restfulipc;
void runAsClient() 
{
    int sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sock < 0) throw C_Error();
    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(struct sockaddr_un));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCK_PATH, strlen(SOCK_PATH));
    if (connect(sock, (const sockaddr*)&addr, sizeof(struct sockaddr_un)) != 0) throw C_Error();
        
    char c = 'a';
    std::cout << "client loop start\n";
    for (int i = 0; i < 100000; i++) {
        if (write(sock, &c, 1) <= 0) throw C_Error();
        if (read(sock, &c, 1) <= 0) throw C_Error();
    }
    std::cout << "client loop end\n";

    close(sock);

}

void runAsServer() 
{
    try {
        struct sockaddr_un sockaddr;
        memset(&sockaddr, 0, sizeof(struct sockaddr_un));
        sockaddr.sun_family = AF_UNIX; 
        strncpy(&sockaddr.sun_path[0], SOCK_PATH, strlen(SOCK_PATH)); 


        int listenSocket = socket(AF_UNIX, SOCK_STREAM, 0);
        if (listenSocket < 0) throw C_Error();
        unlink(SOCK_PATH);
        if (bind(listenSocket, (struct sockaddr*)(&sockaddr), sizeof(sa_family_t) + strlen(SOCK_PATH) + 1) < 0) throw C_Error();
        if (listen(listenSocket, 8) < 0) throw C_Error();
        int connectionSocket = accept(listenSocket, NULL, 0);
        if (connectionSocket < 0) throw C_Error();

        while (true) {
            char c;
            if (recv(connectionSocket, &c, 1, 0) <= 0) {
                throw C_Error();
            }
            if (send(connectionSocket, &c, 1, 0) <= 0) {
                throw C_Error();
            }
        }
    }
    catch (int errorNumber) {
        if (errorNumber > 0) {
            std::cerr << "Error: " << errorNumber << " - " << strerror(errorNumber) << "\n";
            exit(errorNumber);
        }
        else {
            exit(0);
        }
    }
        
}

int main(int argc, char** argv)
{
    if (argc == 2 && !strcmp("server", argv[1])) {
        runAsServer();
    }
    else if (argc == 2 && !strcmp("client", argv[1])) {
        runAsClient();
    }
    else {
        std::cerr << "Usage:\n\n    test_socket_speed server|client\n\n"; 
    }
    return 0;
}

