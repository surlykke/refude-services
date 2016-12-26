
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

#include "errorhandling.h"

#include "fd.h"

namespace refude
{

    Fd::Fd() :
        fd(-1)
    {
    };

    Fd::Fd(int fd):
        fd(fd)
    {
        if (fd < 0) throw C_Error();
    }

    Fd::Fd(Fd&& other)
    {
        fd = other.fd;
        other.fd = -1;
    }

    Fd& Fd::operator=(Fd&& other)
    {
        if (fd > -1) {
            close(fd);
        }
        fd = other.fd;
        other.fd = -1;
    }

    Fd::~Fd() {
        if (fd > -1) {
            while (close(fd) < 0 && errno == EINTR);
        }
    }

    Fd::operator int()
    {
        return fd;
    }

    int Fd::size()
    {
        struct stat fileStat;
        if (fstat(fd, &fileStat) < 0) throw C_Error();
        return fileStat.st_size;
    }
}
