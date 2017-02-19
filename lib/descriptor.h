#ifndef DESCRIPTOR_H
#define DESCRIPTOR_H

#include <fcntl.h>

namespace refude
{
    struct Descriptor
    {
        Descriptor(int fd);
        Descriptor(Descriptor&& other);
        ~Descriptor();

        Descriptor& operator=(Descriptor&& other);
        operator int();
        void close();

        int fd;
    };

}
#endif // DESCRIPTOR_H
