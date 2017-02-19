#include <unistd.h>
#include <sys/stat.h>
#include <sys/sendfile.h>
#include <magic.h>
#include "errorhandling.h"
#include "descriptor.h"
#include "buffer.h"
#include "comm.h"

namespace refude
{
    magic_t initializeMagicCookie()
    {
        magic_t cookie = magic_open(MAGIC_MIME);
        if (!cookie) throw C_Error();
        if (magic_load(cookie, NULL)) {
            throw RuntimeError("Cannot load magic database - %s\n", magic_error(cookie));
        }
    }

    magic_t magic_cookie = initializeMagicCookie();

    void sendBuffer(int socket, const Buffer& buf)
    {
        buf.writeOut(socket);
    }

    void sendFile(int socket, const char* path, const char* mimetype)
    {
        Descriptor file = open(path, O_RDONLY | O_CLOEXEC | O_NOFOLLOW);
        if (file.fd < 0) return sendStatus(socket, HttpCode::Http404);


        struct stat fileStat;
        if (fstat(file, &fileStat) < 0) return sendStatus(socket, HttpCode::Http404);

        if (!mimetype) {
            mimetype = magic_descriptor(magic_cookie, file);

            if (!mimetype) return sendStatus(socket, HttpCode::Http404);
        }

        Buffer headerSection;
        headerSection.write("HTTP/1.1 200 OK\r\n")
                     .write("Content-Type: ").write(mimetype).write("\r\n")
                     .write("Content-Length: ").write((int)fileStat.st_size).write("\r\n")
                     .write("\r\n");

        sendBuffer(socket, headerSection);
        ::sendfile(socket, file, NULL, fileStat.st_size);
    }

    void sendStatus(int socket, HttpCode status)
    {
        Buffer buf;
        buf.write(statusLine(status))
           .write("\r\n");

        sendBuffer(socket, buf);

    }


    void sendString(int socket, const char* str, int size)
    {
        for (int bytesWritten = 0; bytesWritten < size;) {
            int n = write(socket, str + bytesWritten, size - bytesWritten);
            if (n < 0) throw C_Error();
            bytesWritten += n;
        }
    }

}
