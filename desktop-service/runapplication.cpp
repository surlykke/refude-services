
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>

#include <ripc/errorhandling.h>
#include "runapplication.h"

extern char** environ;

namespace org_restfulipc
{
    void runApplication(const char* appAndArguments) 
    {
        pid_t pid = fork();
        if ( pid == 0) {
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

            std::cout << "Doing: ";
            for (char** ptr = argv; *ptr; ptr++) {
                std::cout << *ptr << " ";
            }
            std::cout << "\n";

            execvp(argv[0], argv);
            
            // If we arrive here an error occurred;
            int errorNumber = (errno);
            dprintf(2, "Error: doing execvpe: %s\n", errno);
            exit(errorNumber);
        }
        else if (pid < 0) {
            // In parent process, child creation failed
            throw C_Error();
        }

    }
}