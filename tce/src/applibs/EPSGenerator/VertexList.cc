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
 * @file VertexList.cc
 *
 * Implementation of VertexList class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include <string>
#include "VertexList.hh"

/**
 * The Constructor
 */
VertexList::VertexList() {
}


/**
 * The Destructor.
 */
VertexList::~VertexList() {
}


/**
 * Adds a vertex to the list.
 *
 * @param x X-coordinate of the vertex.
 * @param y Y-coordinate of the vertex.
 */
void
VertexList::addVertex(int x, int y) {
    std::pair<int, int> vertex(x, y);
    vertices_.push_back(vertex);
}

/**
 * Returns vertex count.
 *
 * @return Number of vertices in the list.
 */
size_t
VertexList::size() const {
    return vertices_.size();
}

/**
 * Returns x-coordinate of a vertex with given index.
 *
 * @param index Index of the vertex.
 * @return X-coordinate of the vertex.
 * @exception OutOfRange If the index is out of range.
 */
int
VertexList::vertexX(size_t index) const {
    if (index > size()) {
        std::string error = "Vertex index out of range";
        std::string proc = "VertexList::vertexX";
        OutOfRange e(__FILE__, __LINE__, proc, error);
        throw e;
    }

    int x = vertices_[index].first;

    return x;
}

/**
 * Returns y-coordinate of a vertex with given index.
 *
 * @param index Index of the vertex.
 * @return Y-coordinate of the vertex.
 * @exception OutOfRange If the index is out of range.
 */
int
VertexList::vertexY(size_t index) const {
    if (index > size()) {
        std::string error = "Vertex index out of range";
        std::string proc = "VertexList::vertexY";
        OutOfRange e(__FILE__, __LINE__, proc, error);
        throw e;
    }

    int y = vertices_[index].second;

    return y;
}

/**
 * Removes all vertices from the list.
 */
void
VertexList::clear() {
    vertices_.clear();
}
