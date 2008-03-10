/**
 * @file clashing2.cc
 *
 * File for testing purposes.
 *
 * @author Jussi Nykänen 2004 (nykanen@cs.tut.fi)
 */

// By default, global symbols are exported. Global symbols
// may be declared local with 'static' keyword.
/*static*/ int var = 100;

extern "C" int bar2(int param) {
    return var + 1;
}

