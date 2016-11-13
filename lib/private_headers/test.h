/*
 * Copyright (c) 2015, 2016 Christian Surlykke
 *
 * This file is part of the Refude Services project, library
 * It is distributed under the LGPL 2.1 license.
 * Please refer to the LGPL21 file for a copy of the license.
 */

#ifndef TEST_H
#define    TEST_H

template <int I, int J>
struct Foo;

template <int I>
struct Foo<I, 7> {};

#endif    /* TEST_H */

