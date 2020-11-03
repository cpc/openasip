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
 * @file GraphEdge.hh
 *
 * Prototype of graph-based program representation: declaration of graph
 * edge.
 *
 * @author Andrea Cilio 2005 (cilio-no.spam-cs.tut.fi)
 * @author Vladimir Guzma 2006 (vladimir.guzma-no.spam-tut.fi)
 * @author Heikki Kultala 2008 (hkultala-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_GRAPH_EDGE_HH
#define TTA_GRAPH_EDGE_HH

#include "TCEString.hh"

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
    virtual TCEString toString() const;
    virtual TCEString dotString() const;
    virtual bool isBackEdge() const {
        return false;
    }

    struct Comparator {
        inline bool operator()(GraphEdge* e1, GraphEdge* e2) const {
            return e1->edgeID_ < e2->edgeID_;
        }
    };

    int weight() const { return weight_; }
    void setWeight(int w) { weight_ = w; }
private:
    int edgeID_;
    int weight_;
    static int edgeCounter_;
};

#endif
