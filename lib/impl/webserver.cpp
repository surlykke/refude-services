/*
 * Copyright (c) 2015, 2016 Christian Surlykke
 *
 * This file is part of the Refude Services project, library
 * It is distributed under the LGPL 2.1 license.
 * Please refer to the LGPL21 file for a copy of the license.
 */

#include <limits.h>
#include "httpprotocol.h"
#include "comm.h"
#include "webserver.h"

namespace refude
{
    WebServer::WebServer(const char* rootDir) :
        AbstractResource(),
        rootDir(rootDir)
    {
    }

    void WebServer::doGET(Descriptor& socket, HttpMessage& request, const char *remainingPath)
    {
        char path[PATH_MAX];
        if (! *remainingPath) {
            remainingPath = "/index.html";
        }

        if (snprintf(path, PATH_MAX, "%s%s", rootDir.data(), remainingPath) > PATH_MAX - 1) {
            throw HttpCode::Http404;
        }

        sendFile(socket, path);
    }

}
