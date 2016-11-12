/*
* Copyright (c) 2015, 2016 Christian Surlykke
*
* This file is part of the Restful Inter Process Communication (Ripc) project. 
* It is distributed under the LGPL 2.1 license.
* Please refer to the LICENSE file for a copy of the license.
*/

#ifndef ERRORHANDLING_H
#define    ERRORHANDLING_H

#include <errno.h>
#include <string.h>

#include <stdexcept>
#include <string>
#include <list>
#include <iostream>
#include <sstream>

#include "httpprotocol.h"
#include "httpprotocol.h"

/**
 * We throw 3 types of exceptions:
 *
 *    C_Error
 *      this is a thin wrapper around C's errno, and it is thrown when a c-function
 *      fails ('open', 'read' and such).
 *
 *    Status:
 *      A Http code (4xx, 5xx, ..). This is not necessarily an error, but used when the code figures
 *      out that a http request cannot be served. The status will be catched somewhere
 *      and the corresponding Http response is then sent to the client.
 *
 *    RuntimeError
 *      We use this for pretty much everything else.
 */
namespace refude
{
    struct RuntimeError : public std::exception
    {
        RuntimeError(const char* fmt, ...);
        virtual ~RuntimeError() {};
        virtual const char* what() const noexcept { return errorMsg;}
        void printStackTrace(int fd = 2);
        char errorMsg[1024];
    private:
        void* frames[128];
        int numFrames;
    };

    struct C_Error : public RuntimeError
    {
        C_Error() : RuntimeError(strerror(errno)), errorNumber(errno) {}
        C_Error(const char* info) : RuntimeError("%s: %s", strerror(errno), info), errorNumber(errno) {}
        int errorNumber;
    };
    
    #define C_ERROR_IF(expr) if (expr) throw C_Error();

}


#endif    /* ERRORHANDLING_H */

