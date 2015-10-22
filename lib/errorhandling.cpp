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

    Backtrace::Backtrace() : callStack()
    {
    }

    void Backtrace::saveStackTrace()
    {
        void* frames[100];
        int numFrames = backtrace(frames, 98);
        char **symbols = backtrace_symbols(frames, numFrames);
        for (int i = 3; i < numFrames; i++) {
            // We skip the first two which will be a) the line calling bactrace, above, and b)
            // the constructor of the exception where this method is called.
            callStack.push_back(std::string(symbols[i]));
        }
        if (numFrames >= 100) {
            callStack.push_back(std::string("...")); // To indicate trace may have been truncated.
        }

        delete symbols;

    }

    void Backtrace::printStackTrace(std::ostream &os)
    {
        for (auto iterator = callStack.begin(); iterator != callStack.end(); iterator++) {
            os << *iterator << "\n";
        }
    }

}
