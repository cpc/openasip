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
