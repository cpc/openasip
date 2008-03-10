/**
 * @file foo.cc
 *
 * File for testing purposes.
 *
 * @author Jussi Nykänen 2004 (nykanen@cs.tut.fi)
 */

int var = 5;

extern "C" int bar(int param) {
    return 3 * param;
}

