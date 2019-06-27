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
 * @file VertexList.hh
 *
 * Declaration of VertexList class.
 *
 * @author Veli-Pekka J��skel�inen 2005 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_VERTEX_LIST_HH
#define TTA_VERTEX_LIST_HH

#include <vector>
#include "Exception.hh"

/**
 * VertexList is a class for storing x,y coordinate pairs in a list.
 */
class VertexList {
public:
    VertexList();
    ~VertexList();
    void addVertex(int x, int y);
    size_t size() const;
    int vertexX(size_t index) const;
    int vertexY(size_t index) const;
    void clear();

private:
    /// Copying forbidden.
    VertexList(const VertexList&);
    /// Assignment forbidden.
    VertexList& operator=(const VertexList&);

    /// Vector containing vertices in the list.
    std::vector<std::pair<int, int> > vertices_;
};

#endif
