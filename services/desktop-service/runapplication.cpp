#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>

#include <refude/errorhandling.h>
#include "runapplication.h"

extern char** environ;

namespace org_restfulipc
{
    void runApplication(const char* appAndArguments) 
    {
        pid_t childPid = fork();
        if ( childPid == 0) {
            pid_t grandChildPid = fork();
            if (grandChildPid == 0) {
                setsid(); // Detach
                // In child process. Soon to exit, so we don't care about memory leaks
                char* buf = strdup(appAndArguments);

                // Remove '%u', '%U', '%f' and '%F'
                for (char* c = buf; *c; ) {
                    if (*(c++) == '%' && (*c == 'u' || *c == 'U' || *c == 'f' || *c == 'F')) {
                        *(c - 1) = *c = ' ';
                    }
                }

                char* argv[32]; 
                int argc = 0;

                bool justSawWhitespace = true;
                for (char* c = buf; *c; c++) {
                    if (isspace(*c)) {
                        *c = '\0';
                        justSawWhitespace = true;
                    }
                    else {
                        if (justSawWhitespace) {
                            argv[argc++] = c;
                            justSawWhitespace = false;
                            if (argc >= 31) {
                                throw RuntimeError("Too many arguments: %s\n", appAndArguments);
                            }
                        }
                    }
                }

                argv[argc] = NULL;

                execvp(argv[0], argv);
                 
                // If we arrive here an error occurred;
                int errorNumber = (errno);
                dprintf(2, "Error: doing execvpe: %s\n", errno);
                exit(errorNumber);
            }
            else {
                exit(0);
            }
        }
        else if (childPid < 0) {
            // In parent process, child creation failed
            throw C_Error();
        }
        else { 
	    // In parent process, child creation succeeded 
            int dontCare;
            waitpid(childPid, &dontCare, 0);
        }

    }
}
