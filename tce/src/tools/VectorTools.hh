/*
    Copyright (c) 2002-2009 Tampere University.

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
 * @file VectorTools.icc
 * @author Tero Ryynänen (tero.ryynanen-no.spam-tut.fi) 2008
 *
 * Tools for handling STL Iterative Containers (usually vector).
 *
 * Inline and template definitions.
 *
 */

#ifndef TTA_VECTOR_TOOLS_HH
#define TTA_VECTOR_TOOLS_HH

#include <vector>

#include "Exception.hh"

class VectorTools {
public:
    template <typename ContainerType, typename ValueType>
    static bool containsValue(const ContainerType& aVec, const ValueType& aValue);

    template <typename ContainerType, typename ValueType>
    static bool insertUnique(ContainerType& aVec, const ValueType& aValue);

    template <typename ContainerType, typename ValueType>
    static void removeDuplicates(ContainerType& aVec, const ValueType& aValue);

};

#include "VectorTools.icc"

#endif
