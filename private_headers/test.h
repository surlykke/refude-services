/* 
 * File:   test.h
 * Author: christian
 *
 * Created on 25. juli 2015, 12:51
 */

#ifndef TEST_H
#define    TEST_H

template <int I, int J>
struct Foo;

template <int I>
struct Foo<I, 7> {};

#endif    /* TEST_H */

