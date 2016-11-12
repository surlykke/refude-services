#!/bin/sh

CN=$1
UP=${1^^}
DN=${1,,}
LICENSE=<<LIC
LIC

cat << HEADER > ${DN}.h
/*
 * Copyright (c) 2015, 2016 Christian Surlykke
 *
 * This file is part of the refude-services project. 
 * It is distributed under the GPL v2 license.
 * Please refer to the LICENSE file for a copy of the license.
 */
#ifndef ${UP}_H
#define ${UP}_H
namespace org_restfulipc 
{
    class $CN
    {
    public:
        $CN();
        ~$CN();
    };
}
#endif // ${UP}_H
HEADER
cat << IMPL > ${DN}.cpp
/*
 * Copyright (c) 2015, 2016 Christian Surlykke
 *
 * This file is part of the refude-services project. 
 * It is distributed under the GPL v2 license.
 * Please refer to the LICENSE file for a copy of the license.
 */
#include "${DN}.h"
namespace org_restfulipc 
{
    $CN::$CN()
    {
    }

    $CN::~$CN()
    {
    }
}
IMPL
