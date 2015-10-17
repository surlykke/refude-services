/* 
 * File:   errorhandling.h
 * Author: christian
 *
 * Created on 15. juli 2015, 11:36
 */

#ifndef ERRORHANDLING_H
#define    ERRORHANDLING_H

#include "httpprotocol.h"
#include <errno.h>
#include <string.h>
#include <exception>

#include "httpprotocol.h"

/**
 * We throw 3 types of exceptions:
 *
 *    int:
 *      this is always the value of errno, and it is thrown when a c-function
 *      fails ('open', 'read' and such). Usually it is thrown from assert (below).
 *
 *    Status:
 *      A Http code (4xx and 5xx). This is not necessarily an error, but used when the code figures
 *      out that a http request cannot be served. The status will be catched somewhere
 *      and the corresponding Http response is then sent to the client.
 *
 *    std::runtime_error:
 *      We use this for pretty much everything else.
 */

namespace org_restfulipc
{
    inline void throwErrnoUnless(bool condition) { if (!condition) throw errno; }
    inline void throwHttpStatusUnless(Status status, bool condition) { if (!condition) throw status; }
    void writeBacktrace(int fd = 2);
}


#endif    /* ERRORHANDLING_H */

