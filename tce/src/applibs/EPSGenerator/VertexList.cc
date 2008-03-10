/**
 * @file VertexList.cc
 *
 * Implementation of VertexList class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
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
