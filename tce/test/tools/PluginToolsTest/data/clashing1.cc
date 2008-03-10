/**
 * @file clashing1.cc
 *
 * File for testing purposes.
 *
 * @author Jussi Nykänen 2004 (nykanen@cs.tut.fi) 
 */

int var = 50;

extern "C" int bar1(int param) {
    return var + 1;
}

