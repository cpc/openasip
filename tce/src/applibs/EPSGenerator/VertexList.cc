/*
    Copyright 2002-2008 Tampere University of Technology.  All Rights
    Reserved.

    This file is part of TTA-Based Codesign Environment (TCE).

    TCE is free software; you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 2 as published by the Free
    Software Foundation.

    TCE is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details.

    You should have received a copy of the GNU General Public License along
    with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
    St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
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
VertexList::vertexX(size_t index) const
    throw (OutOfRange) {

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
VertexList::vertexY(size_t index) const
    throw (OutOfRange) {

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
