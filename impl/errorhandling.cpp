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
