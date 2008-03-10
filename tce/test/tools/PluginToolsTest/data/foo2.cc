/**
 * @file foo2.cc
 *
 * File for testing purposes.
 *
 * @author Jussi Nykänen 2004 (nykanen@cs.tut.fi)
 */

int var = 50;
int var2 = 100;

extern "C" int bar(int param) {
    return 1 * param;
}

