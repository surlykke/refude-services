#include <execinfo.h>
#include <stdio.h>
#include "errorhandling.h"

namespace org_restfulipc
{
    void writeBacktrace(int fd)
    {
        void* frames[100];
        int numFrames = backtrace(frames, 100);
        backtrace_symbols_fd(frames, numFrames, fd);
        if (numFrames >= 100) { // Backtrace may have been trunked
            dprintf(fd, "...\n");
        }
    }

}
