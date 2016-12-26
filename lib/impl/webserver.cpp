/*
 * Copyright (c) 2015, 2016 Christian Surlykke
 *
 * This file is part of the Refude Services project, library
 * It is distributed under the LGPL 2.1 license.
 * Please refer to the LGPL21 file for a copy of the license.
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
    WebServer::WebServer(const char* rootDir) :
        AbstractResource(),
        rootFd(open(rootDir, O_CLOEXEC | O_DIRECTORY | O_RDONLY))
    {
        if (!(magic_cookie = magic_open(MAGIC_MIME))) throw C_Error();
        if (magic_load(magic_cookie, NULL)) {
            throw RuntimeError("Cannot load magic database - %s\n", magic_error(magic_cookie));
        }
        // FIXME deallocate in destructor...
    }

    void WebServer::doGET(Fd& requestSocket, HttpMessage& request, Server* server)
    {
        PathMimetypePair pair = findFile(request);
        if (pair.path) {
            Buffer::ptr result = std::make_shared<Buffer>();
            Fd fd = openat(rootFd, pair.path, O_RDONLY | O_CLOEXEC);
            result->writeStr("HTTP/1.1 200 OK\r\n")
                   .writeStr("Content-Type: ").writeStr(pair.mimetype).writeStr("\r\n")
                   .writeStr("Content-Length: ").writeLong(fd.size()).writeStr("\r\n")
                   .writeStr("\r\n");
            result->writeFile(fd.fd);
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
