#ifndef FD_H
#define FD_H

#include <fcntl.h>

namespace refude
{
    struct Fd
    {
    public:
        static Fd open(const char* path, int flags, int mode = -1);
        static Fd open(int dirFd, const char* path, int flags, int mode = -1);

        Fd();
        Fd(int fd);
        Fd(Fd&& other);
        ~Fd();

        Fd& operator=(Fd&& other);
        operator int();

        int size();

        int fd;
    };

}
#endif // FD_H
