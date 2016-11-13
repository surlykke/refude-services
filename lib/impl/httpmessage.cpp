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

#include "httpmessage.h"
#include "errorhandling.h"

namespace refude
{

    HttpMessage::HttpMessage()
    {
    }

    HttpMessage::~HttpMessage()
    {
    }

    const char* HttpMessage::header(const char* headerName)
    {
        int index = headers.find(headerName);
        return index == -1 ? NULL : headers.valueAt(index);
    }

    const char* HttpMessage::parameter(const char* parameterName)
    {
        if (queryParameterMap.contains(parameterName)) {
            return queryParameterMap[parameterName][0];
        }
        else {
            return NULL;
        }
    }


    void HttpMessage::clear()
    {
        method = Method::UNKNOWN;
        status = 0;
        path = 0;
        queryParameterMap.clear();
        headers.clear();
        contentLength = 0;
        body = 0;
        remainingPath = 0;
    }

    HttpMessageReader::HttpMessageReader(int socket, HttpMessage& message, bool dumpRequest) :
        dumpRequest(dumpRequest),
        _socket(socket),
        _message(message),
        _bufferEnd(0),
        _currentPos(-1)
    {
    }

    void HttpMessageReader::readRequest()
    {
        clear();
        readRequestLine();
        readHeaders();
        if (_message.header(Header::content_length)) {
            readBody();
        }
        if (dumpRequest) {
            printf("\n");            
        }
    }

    void HttpMessageReader::readResponse()
    {
        clear();
        readStatusLine();
        readHeaders();
        if (_message.status >= 200 &&
            _message.status != 204 &&
            _message.status != 304) {
            if (_message.header(Header::content_length)) {
                readBody();
            }
        }
    }

    void HttpMessageReader::clear()
    {
        _message.clear();

        _bufferEnd = 0;
        _currentPos = -1;
    }

    void HttpMessageReader::readRequestLine()
    {
        while (nextChar() != ' ');

        _message.method = string2Method(_message.buffer);
        if (_message.method == Method::UNKNOWN) throw HttpCode::Http406;
        _message.path = _message.buffer + _currentPos + 1;

        for (nextChar(); currentChar() != '?' && !isspace(currentChar()); nextChar());

        if (currentChar() == '?') {
            readQueryString();
        }

        _message.buffer[_currentPos] = '\0';

        int protocolStart = _currentPos + 1;

        while (!isspace(nextChar()));

        if (_message.buffer[_currentPos] != '\r' || nextChar() != '\n') throw HttpCode::Http400;
    }

    void HttpMessageReader::readQueryString()
    {
        while (!isspace(currentChar())) {
            _message.buffer[_currentPos++] = '\0';
            char* parameterName = _message.buffer + _currentPos;
            char* parameterValue = NULL;

            while (currentChar() != '=' && currentChar() != '&' && !isspace(currentChar())) {
                nextChar();
            }

            if (currentChar() == '=') {
                _message.buffer[_currentPos++] = '\0';
                parameterValue = _message.buffer + _currentPos;

                while (currentChar() != '&' && !isspace(currentChar())) {
                    nextChar();
                }
            }

            if (parameterValue) {
                _message.queryParameterMap[parameterName].push_back(parameterValue);
            }
            else {
                _message.queryParameterMap[parameterName].push_back("");
            }
        }
    }

    void HttpMessageReader::readStatusLine()
    {
        while (!isspace(nextChar()));
        if (_currentPos <= 0) throw HttpCode::Http400;
        if (strncmp("HTTP/1.1", _message.buffer, 8) != 0) throw HttpCode::Http400;
        while (isspace(nextChar()));
        int statuscodeStart = _currentPos;
        if (!isdigit(currentChar())) throw HttpCode::Http400;
        while (isdigit(nextChar()));
        errno = 0;
        long int status = strtol(_message.buffer + statuscodeStart, 0, 10);
        if (status <= 100 || status >= 600) throw HttpCode::Http400;
        _message.status = (int) status;

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
        while (true) {
            if (nextChar() == '\r') {
                if (nextChar() != '\n') throw HttpCode::Http400;
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
            _message.buffer[_currentPos] = tolower(_message.buffer[_currentPos]);
            nextChar();
        }
        if (currentChar() != ':') throw HttpCode::Http400;
        if (_currentPos <= startOfHeaderLine) throw HttpCode::Http400;
        _message.buffer[_currentPos] = '\0';

        while (isblank(nextChar()));
        endOfHeaderValue = startOfHeaderValue = _currentPos;

        while (currentChar() != '\r') {
            if (!isblank(currentChar())) {
                endOfHeaderValue = _currentPos + 1;
            }
            nextChar();
        }

        if (nextChar() != '\n') throw HttpCode::Http400;
        _message.buffer[endOfHeaderValue] = '\0';
        _message.headers.add(_message.buffer + startOfHeaderLine, _message.buffer + startOfHeaderValue);
    }

    bool HttpMessageReader::isTChar(char c)
    {
        return c != ':'; // FIXME
    }

    void HttpMessageReader::readBody()
    {
        errno = 0;
        _message.contentLength = strtoul(_message.header(Header::content_length), 0, 10);
        if (errno != 0) throw C_Error();

        int bodyStart = _currentPos + 1;
        while (bodyStart + _message.contentLength > _bufferEnd) {
            receive();
        }

        _message.buffer[bodyStart + _message.contentLength] = '\0';
        _message.body = _message.buffer + bodyStart;
    }

    char HttpMessageReader::currentChar()
    {
        return _message.buffer[_currentPos];
    }

    char HttpMessageReader::nextChar()
    {
        _currentPos++;

        while (_currentPos >= _bufferEnd) {
            receive();
        }


        return _message.buffer[_currentPos];
    }

    void HttpMessageReader::receive()
    {
        int bytesRead = read(_socket, _message.buffer + _bufferEnd, 8190 - _bufferEnd);
        if (bytesRead > 0) {
            _message.buffer[_bufferEnd + bytesRead] = '\0';
            if (dumpRequest) {
                printf(_message.buffer + _bufferEnd);
            }
            _bufferEnd += bytesRead;
        }
        else {
            throw C_Error();
        }

    }

    Buffer HttpMessage::toBuf()
    {
        Buffer buf;
        if (path) {
            buf << "HTTP " << method2String(method) << " " << path;
            if (queryParameterMap.size() > 0) {
                char separator = '?';
                for (int i = 0; i < queryParameterMap.size(); i++) {
                    for (const char* value : queryParameterMap.valueAt(i)) {
                        buf << separator << queryParameterMap.keyAt(i) << "=" << value;
                        separator = '&';
                    }
                }
            }
        }
        else {
            buf << status;
        }
        buf << "\n";
        for (int i = 0; i < headers.size(); i++) {
            buf << headers.keyAt(i) << ": " << headers.valueAt(i) << "\n";
        }
        buf << "\n";
        if (body) {
            buf << body << "\n";
        }

        return buf;
    }

    void HttpMessage::setMatchedPathLength(size_t matchedPathLength)
    {
        if (matchedPathLength > strlen(path)) {
            throw RuntimeError("matchedPathLength too big: %d, path length: %d", matchedPathLength, strlen(path));
        }

        remainingPath = path + matchedPathLength;
        if (*remainingPath == '/') {
            remainingPath++;
        }

    }
}

//Method method;
//char* path;
//char* remainingPath;
//char* queryString;
//int status;
//const char* headers[(int) Header::unknown];
//char* body;
//int contentLength;status
//
//char buffer[8192];


