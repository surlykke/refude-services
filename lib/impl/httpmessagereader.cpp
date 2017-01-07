/*
 * Copyright (c) 2015, 2016 Christian Surlykke
 *
 * This file is part of the Refude Services project, library
 * It is distributed under the LGPL 2.1 license.
 * Please refer to the LGPL21 file for a copy of the license.
 */

#include <string.h>
#include <unistd.h>
#include <iosfwd>
#include "errorhandling.h"

#include "httpmessage.h"
#include "httpmessagereader.h"

namespace refude
{

    HttpMessageReader::HttpMessageReader(int socket, bool dumpRequest) :
        dumpRequest(dumpRequest),
        _socket(socket),
        _message(new HttpMessage()),
        _bufferEnd(0),
        _currentPos(-1)
    {
    }

    HttpMessageReader::~HttpMessageReader()
    {
    }

    HttpMessage::ptr HttpMessageReader::readRequest()
    {
        clear();
        readRequestLine();
        readHeaders();
        if (_message->header(Header::content_length)) {
            readBody();
        }
        if (dumpRequest) {
            printf("\n");            
        }

        return std::move(_message);
    }

    HttpMessage::ptr HttpMessageReader::readResponse()
    {
        clear();
        readStatusLine();
        readHeaders();
        if (_message->status >= 200 &&
            _message->status != 204 &&
            _message->status != 304) {
            if (_message->header(Header::content_length)) {
                readBody();
            }
        }

        return std::move(_message);
    }

    void HttpMessageReader::clear()
    {
        _message->clear();

        _bufferEnd = 0;
        _currentPos = -1;
    }

    void HttpMessageReader::readRequestLine()
    {
        while (nextChar() != ' ');

        _message->method = string2Method(_message->buffer);
        if (_message->method == Method::UNKNOWN) throw HttpCode::Http406;
        _message->path = _message->buffer + _currentPos + 1;

        for (nextChar(); currentChar() != '?' && !isspace(currentChar()); nextChar());

        if (currentChar() == '?') {
            readQueryString();
        }

        _message->buffer[_currentPos] = '\0';

        int protocolStart = _currentPos + 1;

        while (!isspace(nextChar()));

        if (_message->buffer[_currentPos] != '\r' || nextChar() != '\n') throw HttpCode::Http400;
    }

    void HttpMessageReader::readQueryString()
    {
        while (!isspace(currentChar())) {
            _message->buffer[_currentPos++] = '\0';
            char* parameterName = _message->buffer + _currentPos;
            char* parameterValue = NULL;

            while (currentChar() != '=' && currentChar() != '&' && !isspace(currentChar())) {
                nextChar();
            }

            if (currentChar() == '=') {
                _message->buffer[_currentPos++] = '\0';
                parameterValue = _message->buffer + _currentPos;

                while (currentChar() != '&' && !isspace(currentChar())) {
                    nextChar();
                }
            }

            if (parameterValue) {
                _message->queryParameterMap[parameterName].push_back(parameterValue);
            }
            else {
                _message->queryParameterMap[parameterName].push_back("");
            }
        }
    }

    void HttpMessageReader::readStatusLine()
    {
        while (!isspace(nextChar()));
        if (_currentPos <= 0) throw HttpCode::Http400;
        if (strncmp("HTTP/1.1", _message->buffer, 8) != 0) throw HttpCode::Http400;
        while (isspace(nextChar()));
        int statuscodeStart = _currentPos;
        if (!isdigit(currentChar())) throw HttpCode::Http400;
        while (isdigit(nextChar()));
        errno = 0;
        long int status = strtol(_message->buffer + statuscodeStart, 0, 10);
        if (status <= 100 || status >= 600) throw HttpCode::Http400;
        _message->status = (int) status;

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
        _message->headers.beginInsert();
        while (true) {
            if (nextChar() == '\r') {
                if (nextChar() != '\n') throw HttpCode::Http400;
                _message->headers.endInsert();
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
        int startOfHeaderLine = _currentPos;
        int startOfHeaderValue = -1;
        int endOfHeaderValue = -1;

        while (isTChar(currentChar())) {
            _message->buffer[_currentPos] = tolower(_message->buffer[_currentPos]);
            nextChar();
        }
        if (currentChar() != ':') throw HttpCode::Http400;
        if (_currentPos <= startOfHeaderLine) throw HttpCode::Http400;
        _message->buffer[_currentPos] = '\0';

        while (isblank(nextChar()));
        endOfHeaderValue = startOfHeaderValue = _currentPos;

        while (currentChar() != '\r') {
            if (!isblank(currentChar())) {
                endOfHeaderValue = _currentPos + 1;
            }
            nextChar();
        }

        if (nextChar() != '\n') throw HttpCode::Http400;
        _message->buffer[endOfHeaderValue] = '\0';
        _message->headers.insert(_message->buffer + startOfHeaderLine, _message->buffer + startOfHeaderValue);
    }

    bool HttpMessageReader::isTChar(char c)
    {
        return c != ':'; // FIXME
    }

    void HttpMessageReader::readBody()
    {
        errno = 0;
        _message->contentLength = strtoul(_message->header(Header::content_length), 0, 10);
        if (errno != 0) throw C_Error();

        int bodyStart = _currentPos + 1;
        while (bodyStart + _message->contentLength > _bufferEnd) {
            receive();
        }

        _message->buffer[bodyStart + _message->contentLength] = '\0';
        _message->body = _message->buffer + bodyStart;
    }

    char HttpMessageReader::currentChar()
    {
        return _message->buffer[_currentPos];
    }

    char HttpMessageReader::nextChar()
    {
        _currentPos++;

        while (_currentPos >= _bufferEnd) {
            receive();
        }


        return _message->buffer[_currentPos];
    }

    void HttpMessageReader::receive()
    {
        ssize_t bytesRead = read(_socket, _message->buffer + _bufferEnd, 8190 - _bufferEnd);
        if (bytesRead > 0) {
            _message->buffer[_bufferEnd + bytesRead] = '\0';
            if (dumpRequest) {
                printf("%s", _message->buffer + _bufferEnd);
            }
            _bufferEnd += bytesRead;
        }
        else {
            throw C_Error();
        }

    }

}


