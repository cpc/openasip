/**
 * @file VertexList.hh
 *
 * Declaration of VertexList class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
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
    int vertexX(size_t index) const
        throw (OutOfRange);
    int vertexY(size_t index) const
        throw (OutOfRange);
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
