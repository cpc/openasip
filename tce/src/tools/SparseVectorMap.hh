/*
    Copyright (c) 2002-2012 Tampere University of Technology.

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
 * @file SparseVectorMap.hh
 *
 * @author Heikki Kultala (heikki.kultala-no.spam-tut.fi) 2012
 *
 * std::map-based container that can act as sparse vector.
 *
 * size() overriden to give index of last element+1 instead
 * number of elements.
 *
 */

#ifndef TTA_SPARSE_VECTOR_MAP_HH
#define TTA_SPARSE_VECTOR_MAP_HH

#include <map>
#include <utility>

template<class ValueType>
class SparseVector: public std::map<unsigned int, ValueType> {
public:
    size_t size() const {
        typename std::map<unsigned int, ValueType>::const_reverse_iterator i = 
            std::map<unsigned int, ValueType>::rbegin();
        if (i != std::map<unsigned int, ValueType>::rend()) {
            return (i->first)+1;
        } else {
            return 0;
        }
    }
    size_t number_of_elements() const {
        return std::map<unsigned int, ValueType>::size();
    }
};

#endif
