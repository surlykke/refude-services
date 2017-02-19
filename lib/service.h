/*
 * Copyright (c) 2015, 2016 Christian Surlykke
 *
 * This file is part of the Refude Services project, library
 * It is distributed under the LGPL 2.1 license.
 * Please refer to the LGPL21 file for a copy of the license.
 */

#ifndef SERVER_H
#define SERVER_H

#include <shared_mutex>
#include <string>
#include "map.h"
#include "abstractresource.h"

namespace refude
{
    namespace service
    {
        void run(int numberOfWorkers = 4);
        void runAndWait(int numberOfWorkers = 4);
        void listen(uint16_t portNumber);
        void listen(std::string socketPath);

    }
}



#endif /* SERVER_H */

