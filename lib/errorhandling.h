/* 
 * File:   errorhandling.h
 * Author: christian
 *
 * Created on 15. juli 2015, 11:36
 */

#ifndef ERRORHANDLING_H
#define    ERRORHANDLING_H

#include <errno.h>
#include <string.h>

#include <stdexcept>
#include <string>
#include <list>
#include <iostream>

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

namespace org_restfulipc
{
    struct Backtrace
    {
        Backtrace();
        void saveStackTrace();
        void printStackTrace(std::ostream& os = std::cerr);
        std::list<std::string> callStack;
    };

    struct RuntimeError : std::runtime_error, Backtrace
    {
        RuntimeError(const char *what) : std::runtime_error(what), Backtrace() {
            saveStackTrace();
        }

        virtual ~RuntimeError() {}
    };

    struct C_Error : public RuntimeError
    {
        C_Error() : RuntimeError(strerror(errno)), errorNumber(errno) {
            saveStackTrace();
        }

        int errorNumber;
    };

}


#endif    /* ERRORHANDLING_H */

