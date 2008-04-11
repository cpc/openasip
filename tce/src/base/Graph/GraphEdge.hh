/**
 * @file GraphEdge.hh
 *
 * Prototype of graph-based program representation: declaration of graph
 * edge.
 *
 * @author Andrea Cilio 2005 (cilio@cs.tut.fi)
 * @author Vladimir Guzma 2006 (vladimir.guzma@tut.fi)
 * @author Heikki Kultala 2008 (hkultala@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_GRAPH_EDGE_HH
#define TTA_GRAPH_EDGE_HH

#include <string>

/**
 * Edge of the graph-based program representation. Each edge identifies a
 * relation (dependency, ordering constraint) between two moves of the
 * program.
 *
 * Edges are pure carriers of information about a node dependency. They do
 * not store head and tail nodes. Edges are independent objects owned by a
 * graph. They do not store a backpointer to their parent graph. All
 * topological information about the graph is stored in ProgramGraph;
 * nothing is distributed across nodes and edges.
 */
class GraphEdge {
public:
    GraphEdge();
    GraphEdge(const GraphEdge& edge);

    virtual GraphEdge* clone() const;
    
    virtual ~GraphEdge();
    virtual int edgeID() const;
    virtual std::string toString() const;
    virtual std::string dotString() const;

    struct Comparator {
        bool operator()(GraphEdge* e1, GraphEdge* e2) const;
    };

private:
    int edgeID_;
    static int edgeCounter_;
};

#endif
