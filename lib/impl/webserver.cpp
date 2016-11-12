/*
* Copyright (c) 2015, 2016 Christian Surlykke
*
* This file is part of the Restful Inter Process Communication (Ripc) project. 
* It is distributed under the LGPL 2.1 license.
* Please refer to the LICENSE file for a copy of the license.
*/

#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/sendfile.h>
#include <stdio.h>
#include <unistd.h>
#include <magic.h>
#include "httpprotocol.h"
#include "errorhandling.h"
#include "webserver.h"

namespace refude
{
    struct FileWriter
    {
        FileWriter(int socket, int dirFd, const char* filePath, const char* mimetype) :
            socket(socket),
            fd(0),
            mimetype(mimetype)
        {
            if (*filePath == '/') {
                filePath++;
            }
            fd = openat(dirFd, filePath, O_RDONLY | O_CLOEXEC | O_NOFOLLOW);
            if (fd < 0) {
                if (errno == ENOENT) {
                    throw HttpCode::Http404;
                }
                else {
                    throw C_Error();
                }
            }
        }

        ~FileWriter()
        {
            while (close(fd) < 0 && errno == EINTR);
        }

        void writeFile() {
            struct stat fileStat;
            if (fstat(fd, &fileStat) < 0) throw C_Error();
            int filesize = fileStat.st_size;
            static const char* headerTemplate =
                    "HTTP/1.1 200 OK\r\n"
                    "Content-Type: %s\r\n"
                    "Content-Length: %d\r\n"
                    "\r\n";
            char header[strlen(headerTemplate) + 256];
            sprintf(header, headerTemplate, mimetype, filesize);
            int bytesTotal = strlen(header);
            for (int byteswritten = 0;  byteswritten < bytesTotal; ) {
                int bytes = write(socket, header, bytesTotal - byteswritten);
                if (bytes < 0) throw C_Error();
                byteswritten += bytes;
            }

            off_t offset = 0L;
            while (filesize > 0)  {
                int bytes = sendfile(socket, fd, &offset, filesize);
                if (bytes < 0) throw C_Error();
                filesize -= bytes;
            }

        }

        int socket;
        int fd;
        const char* mimetype;
    };

    WebServer::WebServer(const char* rootDir) :
        AbstractResource(),
        rootDir(rootDir),
        rootFd(-1)
    {
        rootFd = open(rootDir, O_CLOEXEC | O_DIRECTORY | O_RDONLY);
        if (rootFd < 0) throw C_Error();
        if (!(magic_cookie = magic_open(MAGIC_MIME))) throw C_Error();
        if (magic_load(magic_cookie, NULL)) {
            throw RuntimeError("Cannot load magic database - %s\n", magic_error(magic_cookie));
        }
        // FIXME deallocate in destructor...
    }
    void WebServer::doGET(int& socket, HttpMessage& request)
    {
        PathMimetypePair pair = findFile(request);
        if (pair.path) {
            FileWriter(socket, rootFd, pair.path, pair.mimetype).writeFile();
        }
        else { 
            throw HttpCode::Http404;
        }
    }

    PathMimetypePair WebServer::findFile(HttpMessage& request)
    {
        PathMimetypePair resp;

        if (request.remainingPath[0] == '\0') { 
            resp.path = "index.html";
        }
        else {
            resp.path = request.remainingPath;
        }
        
        // TODO Maybe we can avoid a (costly?) call to magic_file by looking at fileendings... 
        resp.mimetype = magic_file(magic_cookie, resp.path);        

        return resp;
    }

}
