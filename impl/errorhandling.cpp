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
#include <unistd.h>
#include <limits.h>
#include <dlfcn.h>

#include "errorhandling.h"

namespace org_restfulipc
{
    RuntimeError::RuntimeError(const char* fmt, ...)
    {
        numFrames = backtrace(frames, 128);
        va_list args;
        va_start(args, fmt);
        std::vsprintf(errorMsg, fmt, args);
        va_end(args);
    }

    void RuntimeError::printStackTrace(int fd)
    {
        char procPidPath[PATH_MAX];
        sprintf(procPidPath, "/proc/%d/exe", getpid());
        char path2executable[PATH_MAX];
        if (!realpath(procPidPath, path2executable)) {
            dprintf(fd, "Unable to determine executable..");
            return;
        }
        const char* lineTemplate = "  ##--## %s:%x\n";
        dprintf(fd, "Stacktrace:\n");
        for (int i = 0; i < numFrames; i++) {
            long address = (long) frames[i];
           
            Dl_info dl_info;
            if (!dladdr(frames[i], &dl_info)) {
                dprintf(fd, "Unable to determine address\n");
            }
           
            const char* commandTemplate = "addr2line -e %s 0x%x";
            char command[1024];
            if ((long)dl_info.dli_fbase == 0x400000) {
                snprintf(command, 1023, commandTemplate, path2executable, address);
            }
            else {
                snprintf(command, 1023, commandTemplate,  dl_info.dli_fname, address - (long)dl_info.dli_fbase);
            }
            FILE* file = popen(command, "r");
            if (file) {
                char addr2lineOutput[1024];
                dprintf(fd, "  %s", fgets(addr2lineOutput, 1023, file));
                fclose(file);
            }
        }
        dprintf(fd, "\n");
    }
}
