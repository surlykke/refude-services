#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/sendfile.h>
#include <stdio.h>
#include <unistd.h>

#include "httpprotocol.h"
#include "errorhandling.h"
#include "webserver.h"

namespace org_restfulipc
{
    struct FileWriter
    {
        FileWriter(int socket, int dirFd, const char* filePath) :
            socket(socket),
            fd(0)
        {
            std::cout << "serving: " << filePath << "\n";
            if (*filePath == '/') {
                filePath++;
            }
            fd = openat(dirFd, filePath, O_RDONLY | O_CLOEXEC | O_NOFOLLOW);
            std::cout << "fd: " << fd << "\n";
            if (fd < 0) {
                if (errno == ENOENT) {
                    throw Status::Http404;
                }
                else {
                    throw C_Error();
                }
            }

        }

        ~FileWriter()
        {
            std::cout << "destructor\n";
            while (close(fd) < 0 && errno == EINTR);
        }

        void writeFile() {
            struct stat fileStat;
            if (fstat(fd, &fileStat) < 0) throw C_Error();
            int filesize = fileStat.st_size;
            std::cout << "filesize: " << fileStat.st_size << "\n";
            static const char* headerTemplate =
                    "HTTP/1.1 200 OK\r\n"
                    "Content-Type: %s\r\n"
                    "Content-Length: %d\r\n"
                    "\r\n";
            char header[strlen(headerTemplate) + 256];
            sprintf(header, headerTemplate, "text/html; charset=UTF-8", filesize);
            int bytesTotal = strlen(header);
            for (int byteswritten = 0;  byteswritten < bytesTotal; ) {
                std::cout << "write(" <<  header << ", " << (bytesTotal - byteswritten) << ")\n";
                int bytes = write(socket, header, bytesTotal - byteswritten);
                if (bytes < 0) throw C_Error();
                byteswritten += bytes;
            }

            off_t offset = 0L;
            while (filesize > 0)  {
                std::cout << "writing body, socket: " << socket << ", fd: " << fd << ", filesize: " << filesize << "\n";
                int bytes = sendfile(socket, fd, &offset, filesize);
                if (bytes < 0) throw C_Error();
                filesize -= bytes;
            }

        }

        int socket;
        int fd;
    };

    WebServer::WebServer(const char* html_root) :
        AbstractResource(),
        rootFd(-1)
    {
        std::cout << "WebServer at: " << html_root << "§\n";
        rootFd = open(html_root, O_CLOEXEC | O_DIRECTORY | O_NOATIME | O_RDONLY);
        if (rootFd < 0) throw C_Error();
    }

    void WebServer::handleRequest(int& socket, const HttpMessage& request)
    {
        const char* filePath;
        if (strcmp("", request.remainingPath) == 0 || strcmp("/", request.remainingPath) == 0) {
            filePath = "index.html";
        }
        else {
            filePath = request.remainingPath;
        }

        FileWriter(socket, rootFd, filePath).writeFile();
    }


}
