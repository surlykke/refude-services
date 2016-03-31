/*
* Copyright (c) 2015, 2016 Christian Surlykke
*
* This file is part of the Restful Inter Process Communication (Ripc) project. 
* It is distributed under the LGPL 2.1 license.
* Please refer to the LICENSE file for a copy of the license.
*/

#include <execinfo.h>
#include <stdio.h>
#include <stdarg.h>
#include "errorhandling.h"

namespace org_restfulipc
{
    RuntimeError::RuntimeError(const char* fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
        std::vsprintf(errorMsg, fmt, args);
        va_end(args);
    }

}
