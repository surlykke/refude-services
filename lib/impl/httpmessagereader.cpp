/*
 * Copyright (c) 2015, 2016 Christian Surlykke
 *
 * This file is part of the Refude Services project, library
 * It is distributed under the LGPL 2.1 license.
 * Please refer to the LGPL21 file for a copy of the license.
 */

#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <iosfwd>

#include "errorhandling.h"
#include "httpmessagereader.h"

namespace refude
{

    HttpMessageReader::HttpMessageReader(bool dumpRequest) :
        dumpRequest(dumpRequest),
        msg(0),
        socket(-1),
        currentPos(-1),
        bufferEnd(0)
    {
    }

    void HttpMessageReader::readRequest(HttpRequest* request, int socket)
    {
        msg = request;
        this->socket = socket;
        currentPos = -1;
        bufferEnd = 0;

        readRequestLine();

        readHeaders();

        if (msg->header(Header::content_length)) {
            readBody();
        }

        if (dumpRequest) {
            printf("\n");            
        }
    }

    void HttpMessageReader::readResponse(HttpResponse* response, int socket)
    {
        msg = response;
        this->socket = socket;
        readStatusLine();
        readHeaders();
        if (msg->status >= 200 &&
            msg->status != 204 &&
            msg->status != 304) {
            if (msg->header(Header::content_length)) {
                readBody();
            }
        }
    }

    void HttpMessageReader::readRequestLine()
    {
        while (nextChar() != ' ');

        msg->method = string2Method(msg->buffer);

        if (msg->method == Method::UNKNOWN) throw HttpCode::Http406;
        msg->path = msg->buffer + currentPos + 1;

        for (nextChar(); currentChar() != '?' && !isspace(currentChar()); nextChar());

        if (currentChar() == '?') {
            readQueryString();
        }

        msg->buffer[currentPos] = '\0';

        int protocolStart = currentPos + 1;

        while (!isspace(nextChar()));

        if (msg->buffer[currentPos] != '\r' || nextChar() != '\n') throw HttpCode::Http400;
    }

    void HttpMessageReader::readQueryString()
    {
        while (!isspace(currentChar())) {
            msg->buffer[currentPos++] = '\0';
            char* parameterName = msg->buffer + currentPos;
            char* parameterValue = NULL;

            while (currentChar() != '=' && currentChar() != '&' && !isspace(currentChar())) {
                nextChar();
            }

            if (currentChar() == '=') {
                msg->buffer[currentPos++] = '\0';
                parameterValue = msg->buffer + currentPos;

                while (currentChar() != '&' && !isspace(currentChar())) {
                    nextChar();
                }
            }

            if (parameterValue) {
                msg->queryParameterMap[parameterName].push_back(parameterValue);
            }
            else {
                msg->queryParameterMap[parameterName].push_back("");
            }
        }
    }

    void HttpMessageReader::readStatusLine()
    {
        while (!isspace(nextChar()));
        if (currentPos <= 0) throw HttpCode::Http400;
        if (strncmp("HTTP/1.1", msg->buffer, 8) != 0) throw HttpCode::Http400;
        while (isspace(nextChar()));
        int statuscodeStart = currentPos;
        if (!isdigit(currentChar())) throw HttpCode::Http400;
        while (isdigit(nextChar()));
        errno = 0;
        long int status = strtol(msg->buffer + statuscodeStart, 0, 10);
        if (status <= 100 || status >= 600) throw HttpCode::Http400;
        msg->status = (int) status;

        // We ignore what follows the status code. This means that a message like
        // 'HTTP/1.1 200 Completely f**cked up' will be interpreted as 
        // 'HTTP/1.1 200 Ok'
        // (Why does the http protocol specify that both the code and the text is sent?)
        while (currentChar() != '\r') {
            if (currentChar() == '\n') throw HttpCode::Http400;
            nextChar();
        }

        if (nextChar() != '\n') throw HttpCode::Http400;
    }

    // On entry: currentPos points to character just before next header line

    void HttpMessageReader::readHeaders()
    {
        msg->headers.beginInsert();
        while (true) {
            if (nextChar() == '\r') {
                if (nextChar() != '\n') throw HttpCode::Http400;
                msg->headers.endInsert();
                return;
            }

            readHeaderLine();
        }
    }

    /* On entry - _currentPos points to first character of line
     * 
     * TODO: Full implementation of spec
     *  - multiline header definitions
     *  - Illegal chars in names/values
     */
    void HttpMessageReader::readHeaderLine()
    {
        int startOfHeaderLine = currentPos;
        int startOfHeaderValue = -1;
        int endOfHeaderValue = -1;

        while (isTChar(currentChar())) {
            msg->buffer[currentPos] = tolower(msg->buffer[currentPos]);
            nextChar();
        }
        if (currentChar() != ':') throw HttpCode::Http400;
        if (currentPos <= startOfHeaderLine) throw HttpCode::Http400;
        msg->buffer[currentPos] = '\0';

        while (isblank(nextChar()));
        endOfHeaderValue = startOfHeaderValue = currentPos;

        while (currentChar() != '\r') {
            if (!isblank(currentChar())) {
                endOfHeaderValue = currentPos + 1;
            }
            nextChar();
        }

        if (nextChar() != '\n') throw HttpCode::Http400;
        msg->buffer[endOfHeaderValue] = '\0';
        msg->headers.insert(msg->buffer + startOfHeaderLine, msg->buffer + startOfHeaderValue);
    }

    bool HttpMessageReader::isTChar(char c)
    {
        return c != ':'; // FIXME
    }

    void HttpMessageReader::readBody()
    {
        errno = 0;
        msg->contentLength = strtoul(msg->header(Header::content_length), 0, 10);
        if (errno != 0) throw C_Error();

        int bodyStart = currentPos + 1;
        while (bodyStart + msg->contentLength > bufferEnd) {
            receive();
        }

        msg->buffer[bodyStart + msg->contentLength] = '\0';
        msg->body = msg->buffer + bodyStart;
    }

    char HttpMessageReader::currentChar()
    {
        return msg->buffer[currentPos];
    }

    char HttpMessageReader::nextChar()
    {
        currentPos++;

        while (currentPos >= bufferEnd) {
            receive();
        }


        return msg->buffer[currentPos];
    }

    void HttpMessageReader::receive()
    {
        int bytesRead = read(socket, msg->buffer + bufferEnd, 8190 - bufferEnd);

        if (bytesRead > 0) {
            msg->buffer[bufferEnd + bytesRead] = '\0';
            if (dumpRequest) {
                printf(msg->buffer + bufferEnd);
            }
            bufferEnd += bytesRead;
        }
        else {
            throw C_Error();
        }
    }
}
