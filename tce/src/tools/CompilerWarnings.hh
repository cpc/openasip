/*
    Copyright (c) 2015 Tampere University.

    This file is part of TTA-Based Codesign Environment (TCE).

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
 */
/**
 * @file CompilerWarnings.hh
 *
 * Macros to control compiler warnings.
 *
 * @author Pekka Jääskeläinen 2015
 */

#ifndef TCE_COMPILER_WARNINGS_HH
#define TCE_COMPILER_WARNINGS_HH

#define DO_PRAGMA(x) _Pragma(#x)

#ifdef __clang__

#define IGNORE_COMPILER_WARNING(X)                        \
    DO_PRAGMA(clang diagnostic push)                      \
    DO_PRAGMA(clang diagnostic ignored X)                 

#elif defined(__GNUC__)

#define IGNORE_COMPILER_WARNING(X)                      \
    DO_PRAGMA(GCC diagnostic push)                      \
    DO_PRAGMA(GCC diagnostic ignored X)                 

#else

#define IGNORE_COMPILER_WARNING(X) 

#endif


#ifdef __clang__

#define POP_COMPILER_DIAGS \
    DO_PRAGMA(clang diagnostic pop)

#elif defined(__GNUC__)

#define POP_COMPILER_DIAGS \
    DO_PRAGMA(GCC diagnostic pop)

#else

#define POP_COMPILER_DIAGS 

#endif

// Macro to suppress warnings for clang only. In some cases the
// IGNORE_COMPILER_WARNING is not preferred for both the GCC and clang. For
// example, clang has own warning enabled with -Wall/-Wextra or warning option
// differs slightly (-Wunused-local-typedef vs -Wunused-local-typedefs).
// Use POP_CLANG_DIAGS to remove the diagnostic.
#ifdef __clang__

#define IGNORE_CLANG_WARNING(X)                           \
    DO_PRAGMA(clang diagnostic push)                      \
    DO_PRAGMA(clang diagnostic ignored X)

#else

#define IGNORE_CLANG_WARNING(X)

#endif

#ifdef __clang__

#define POP_CLANG_DIAGS \
    DO_PRAGMA(clang diagnostic pop)

#else

#define POP_CLANG_DIAGS

#endif

#endif

