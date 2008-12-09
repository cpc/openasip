/*
    Copyright (c) 2002-2009 Tampere University of Technology.

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
 * @file SetTools.hh
 *
 * Tools for handling STL Sets.
 *
 * Declarations.
 *
 * @author Heikki Kultala 2008 (hkultala-no.spam-cs.tut.fi)
 */

#ifndef TTA_SET_TOOLS_HH
#define TTA_SET_TOOLS_HH

#include <set>
#include <utility>

#include "Exception.hh"

class SetTools {
public:
    template <typename ValueType>
    static void intersection(
        const std::set<ValueType>& firstContainer,
        const std::set<ValueType>& secondContainer,
        std::set<ValueType>& intersection);

    template <typename ValueType, typename Comparator>
    static void intersection(
        const std::set<ValueType, Comparator>& firstContainer,
        const std::set<ValueType, Comparator>& secondContainer,
        std::set<ValueType, Comparator>& intersection);
};

#include "SetTools.icc"
#endif
