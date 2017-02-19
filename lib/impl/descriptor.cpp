#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

#include "errorhandling.h"

#include "descriptor.h"

namespace refude
{

    Descriptor::Descriptor(int fd):
        fd(fd)
    {
    }

    Descriptor::Descriptor(Descriptor&& other)
    {
        fd = other.fd;
        other.fd = -1;
    }

    Descriptor& Descriptor::operator=(Descriptor&& other)
    {
        if (fd > -1) close();
        fd = other.fd;
        other.fd = -1;
    }

    Descriptor::~Descriptor() {
        if (fd > -1) close();
    }

    Descriptor::operator int()
    {
        return fd;
    }

    void Descriptor::close()
    {
        while (::close(fd) < 0 && errno == EINTR);
        fd = -1;
    }
}
